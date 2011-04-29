#include <reflect/Class.h>
#include <reflect/autocast.h>
#include <reflect/execute/ApplicationClass.h>
#include <reflect/string/String.h>
#include <reflect/PrimitiveTypes.h>
#include <cstdio>

#if defined(_MSC_VER) || defined(_WIN32)
# define WINDOWS
#endif

#if defined(WINDOWS)
# include <windows.h>
# define LoadSharedObject(name) LoadLibrary(name)
# define SharedObjectHandle HINSTANCE
#elif defined(__GNUC__)
# include <dlfcn.h>
typedef void *SharedObjectHandle__;
# define SharedObjectHandle SharedObjectHandle__
# define LoadSharedObject(name) dlopen(name, RTLD_GLOBAL | RTLD_LAZY)
#else
# error TODO: Define shared object functions for other platforms.
#endif

using namespace reflect;

static void DebugLoadedTypes(string::ConstString module, Type *loaded)
{
	fprintf(stderr, "Module %s Classes:\n", module.c_str());
	
	for(Type *it = loaded; it != NULL; it = it->NextTypeToLoad())
	{
		fprintf(stderr, "\t%s [%s]\n", it->Name(), it->GetClass()->Name());
	}
}

struct TypeTreePrintNode
{
	TypeTreePrintNode(TypeTreePrintNode *p, bool l)
		: prev(p)
		, line(l)
	{}
	
	TypeTreePrintNode *prev; bool line; 

	void print() const
	{
		if(prev) prev->print();
		if(line) printf("  |"); else printf("   ");
	}
};

static int PrintTypeTree(Type *clazz = Dynamic::TheClass(), TypeTreePrintNode *node = 0)
{
	printf("%s('%s')\r\n", clazz->GetClass()->Name(), clazz->Name());

	if(Type *child = clazz->Child())
	{
		TypeTreePrintNode localnode(node, true);
		do
		{
			localnode.print();
			printf("\r\n");

			if(node) node->print();
			printf("  +-");

			if(child->Sibling() == clazz->Child())
			{
				localnode.line = false;
			}
			PrintTypeTree(child, &localnode);
			//for(int i = 0; i < depth-1; i++) printf("%s", i < commitdepth ? "   " : "  |");
			//printf("\r\n");
		}
		while((child = child->Sibling()) != clazz->Child());
	}
	
	return 0;
}

struct Main
{
	bool optDebug;

	Main() : optDebug(false) {}

	int Process(int argc, char *argv[]);
	
	void Load(string::ConstString libname)
	{
		if(SharedObjectHandle so = LoadSharedObject(libname.c_str()))
		{
			(void)so;

			Type *loaded = Class::LoadTypes();

			if(optDebug)
			{
				DebugLoadedTypes(libname, loaded);
			}
		}
		else
		{
			fprintf(stderr, "Could not find module %s\n", libname.c_str());
		}
	}

	int Execute(string::ConstString applicationclass, int argc, char *argv[])
	{
		//PrintTypeTree();
		//PrintTypeTree(Signature<void>::TheClass());
				
		execute::ApplicationClass *exec = Type::FindType(applicationclass.c_str()) % autocast;

		if(exec)
		{
			return exec->Execute(argc, argv);
		}
		else
		{
			fprintf(stderr, "ERROR: ApplicationClass '%s' not registered.\n", applicationclass.c_str());
			return 1;
		}
	}
};

// Function: main
//
// The entry point into reflect.
// 
// This function calls <Type::LoadTypes> to initialize reflects
// built-in classes, and then processes the command line.
//
// Usage: 
//
// Any number of commands, ending with an --execute or x:y command.
//
//   --load plugin - loads the plugin and calls LoadTypes to initialize it.
//   --execute ClassName - executes the <Application> class named ClassName.
//   plugin:ClassName - alias for --load plugin --execute plugin::ClassName 
//        (note the one colon turning into two).
//   --debug - turns on debugging.
//   --show-classes - prints the current class tree.
int main(int argc, char *argv[])
{
	Type *loaded = Type::LoadTypes();

	//DebugLoadedTypes("reflect", loaded);
	(void)loaded; // fix warning when above spam is commented out.

	Main m;

	int result = m.Process(argc, argv);

	return result;
}

int Main::Process(int argc, char *argv[])
{
	int result = 0;

	for(int arg = 1; arg < argc; arg++)
	{
		string::ConstString option = argv[arg];

		if(option == "--debug")
		{
			optDebug = true;
		}
		
		if(option == "--show-classes")
		{
			PrintTypeTree();
			//printf("");
		}
		
		if(option.find("--show-classes=") == 0)
		{
			PrintTypeTree(Type::FindType(option.substr(15).data()));
		}

		if(option == "--load")
		{
			if(const char *libname = argv[++arg])
			{		
				Load(libname);
			}
			else
			{
				fprintf(stderr, "--load requires an argument\n");
				result = 1;
				break;
			}
		}
		else if(option == "--execute")
		{
			if(const char *what = argv[++arg])
			{
				char *oldarg = argv[arg];
				argv[arg] = argv[0];
				result = Execute(what, argc - arg, argv + arg);
				argv[arg] = oldarg;
				break;
			}
			else
			{
				fprintf(stderr, "--execute requires an argument\n");
				result = 1;
				break;
			}
		}
		else if(option.find(':') != string::ConstString::npos)
		{
			char *part0 = argv[arg];
			char *part1 = argv[arg] + option.find(':') + 1;
			part1[-1] = '\0';

			string::String application_class;
			application_class << part0 << "::" << part1;

			Load(part0);
			argv[arg] = argv[0];
			result = Execute(application_class, argc - arg, argv + arg);
			break;
		}
	}

	return result;
}

#if defined(_WIN32) || defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int __stdcall WinMain(      
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/) 
{
	int argc;
	int index;
	wchar_t **argvw;
	char **argv;

	argvw = CommandLineToArgvW(GetCommandLineW(), &argc);

	argv = new char *[argc+1];
	argv[argc] = NULL;

	for(index = 0; index < argc; index++)
	{	
		size_t argsize;
		(void)wcstombs_s(&argsize, NULL, 0, argvw[index], _TRUNCATE);
		argv[index] = new char[argsize+1];
		(void)wcstombs_s(NULL, argv[index], argsize+1, argvw[index], _TRUNCATE);
	}

	{
		int main(int, char **);
		return main(argc, argv);
	}
}
#endif
