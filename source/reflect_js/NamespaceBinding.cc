#include <reflect_js/JavaScript_private.h>
#include <reflect/string/Fragment.h>
#include <reflect/string/SharedString.h>
#include <set>
#include <reflect/ObjectType.hpp>

namespace /*private*/ {

struct Empty {};

struct NamespaceTree
{
	struct Node
	{
		typedef std::map<reflect::string::SharedString, Node> NodeSet;
		NodeSet mChildren;
		
		Node()
			: mChildren()
			, mType(0)
		{}
		
		Node &AddChild(reflect::string::SharedString name)
		{
			return mChildren[name];
		}
		
		void SetType(const reflect::Type *type)
		{
			mType = type;
		}
		
		void Create() const
		{
			if(mType)
			{
				if(false == mType->Abstract())
				{
					// don't use variant's Construct,
					//   we can't let the variant own the allocation.
					reflect::Variant result =
						reflect::Variant::FromOpaque(mType, mType->Construct(new char[mType->Size()]));

					reflect::js::JavaScriptContext::CallContext()->DecOpaqueRef(result.Opaque());

					if(1 == reflect::js::JavaScriptContext::CallArgumentCount())
					{
						reflect::js::JavaScriptContext::CallArgument(0, result);
					}
					
					reflect::js::JavaScriptContext::CallContext()->CallReturn(result);
				}
			}
		}
		
		const reflect::Type *mType;
	};

	Node *AddNode(reflect::string::Fragment name)
	{
		Node *node = 0;
		Node::NodeSet *nodeset = &mRootSet;
		
		using namespace reflect::string;
		
		while(name.size())
		{
			Fragment::size_type pos = name.find_first_of(":.", 0);
			
			Fragment piece = name.substr(0, pos);

			if(Fragment::npos == pos) name = "";
			else name = name.substr(pos + 1);
			
			if(piece.size())
			{
				node = &(*nodeset)[SharedString::Copy(piece)];
				nodeset = &node->mChildren;
			}
		}

		return node;
	}

	Node::NodeSet mRootSet;
};

}

DECLARE_STATIC_REFLECTION(local, NamespaceTree::Node, reflect::ObjectType)
DEFINE_STATIC_REFLECTION(NamespaceTree::Node, "")
{
}

static NamespaceTree *sNamespaceTree = 0;

static void EnumerateNamespaceTree(const reflect::Type *root_peer)
{
	if(root_peer)
	{
		const reflect::Type *sibling = root_peer;
		
		do
		{
			EnumerateNamespaceTree(sibling->Child());
			if(sibling->Name() && *sibling->Name())
			{
				NamespaceTree::Node *node =
					sNamespaceTree->AddNode(sibling->Name());

				if(node)
				{
					node->SetType(sibling);
				}
				else
				{
					fprintf(stderr, "%s unregistered\n", sibling->Name());
				}
			}
			sibling = sibling->Sibling();
		}	
		while(sibling != root_peer);
	}
}

static void InitNamespaceTree()
{
	if(sNamespaceTree != 0)
		return;
	
	sNamespaceTree = new NamespaceTree();
	
	EnumerateNamespaceTree(reflect::Type::AnyRootType());
}

static JSBool EnumerateNamespace(JSContext *, JSObject *);
static JSBool ResolveNamespace(JSContext *, JSObject *, jsval id);

static JSClass sNamespaceClass = {
	"reflect::Namespace",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, // add
	JS_PropertyStub, // del
	JS_PropertyStub, // get
	JS_PropertyStub, // set
	EnumerateNamespace, 
	ResolveNamespace,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

namespace reflect { namespace js {
JSClass *NamespaceClass() { return &sNamespaceClass; }
} }

template<typename FunctionType>
static JSBool Invoke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval, FunctionType function, reflect::Variant self)
{
	reflect::function::FunctionImpl<FunctionType> native("", function);
	
	return reflect::js::InvokeNative(cx, argc, argv, rval, &native, self);
}

JSBool NewType(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	jsval function_value = argv[-2];
	jsval slot;
	JS_GetReservedSlot(cx, JS_GetFunctionObject(JS_ValueToFunction(cx, function_value)), 0, &slot);
	const NamespaceTree::Node *node = static_cast<const NamespaceTree::Node *>(JSVAL_TO_PRIVATE(slot));
	return Invoke(cx, obj, argc, argv, rval, &NamespaceTree::Node::Create, reflect::Variant::FromConstRef(*node));
}

static JSBool EnumerateNamespaceInObject(JSContext *cx, JSObject *obj, NamespaceTree::Node::NodeSet *nodeset)
{
	reflect::js::RuntimeData *rtdata = reflect::js::GetRuntimeData(JS_GetRuntime(cx));
	
	for(NamespaceTree::Node::NodeSet::iterator it = nodeset->begin();
		it != nodeset->end();
		++it)
	{
		JSBool found = false;
		
		if(JS_HasProperty(cx, obj, it->first.c_str(), &found))
		{
			if(!found)
			{
				NamespaceTree::Node *node = &it->second;
				
				if(node->mType)
				{
					JSFunction *function = JS_DefineFunction(cx, obj, it->first.c_str(), NewType, 0, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
					JS_SetReservedSlot(cx, JS_GetFunctionObject(function), 0, PRIVATE_TO_JSVAL(node));
					JSObject *proto = 0;
					
					{
						jsval protoval;
						JS_GetProperty(cx, JS_GetFunctionObject(function), "prototype", &protoval);
						JS_ValueToObject(cx, protoval, &proto);
					}
			
					rtdata->SetPrototype(node->mType, proto);
					
					for(reflect::ObjectType::FunctionIterator it(node->mType % reflect::autocast); it; it.next())
					{
						if(!it->second->IsMethod())
						{
							reflect::js::DefineNativeFunction(cx, JS_GetFunctionObject(function), it->second);
		//					JSFunction *subfunction = JS_DefineFunction(cx, JS_GetFunctionObject(function), it->first.c_str(),
		//						reflect::js::CallNativeFunction, it->second->NumParameters(),
		//						JSPROP_READONLY | JSPROP_ENUMERATE);

		//					JS_SetReservedSlot(cx, JS_GetFunctionObject(subfunction), 0, PRIVATE_TO_JSVAL(it->second));
						}
						else
						{	
							reflect::js::DefineNativeFunction(cx,
								proto,
								it->second);
						}
					}
				}
				else
				{
					JSObject *subnamespace = JS_NewObject(cx, reflect::js::NamespaceClass(), 0, obj);
					JS_SetPrivate(cx, subnamespace, node);
					JS_DefineProperty(cx, obj, it->first.c_str(), OBJECT_TO_JSVAL(subnamespace),
						0, 0, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
				}
			}
		}
	}
	
	return JS_TRUE;	
}

static JSBool EnumerateNamespace(JSContext *cx, JSObject *obj)
{
	NamespaceTree::Node *node = static_cast<NamespaceTree::Node *>(JS_GetPrivate(cx, obj));

	return EnumerateNamespaceInObject(cx, obj, &node->mChildren);
}

static JSBool ResolveNamespace(JSContext *cx, JSObject *obj, jsval id)
{
	(void)id;
	JS_Enumerate(cx, obj);

	return JS_TRUE;
}

namespace reflect { namespace js {
JSBool EnumerateRootNamespaces(JSContext *cx, JSObject *obj)
{
	InitNamespaceTree();
	
	return EnumerateNamespaceInObject(cx, obj, &sNamespaceTree->mRootSet);
}

} }
