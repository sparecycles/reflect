pushd %~dp0

mkdir .\doc
mkdir .\doc\reflect-docs

call ext\naturaldocs\NaturalDocs.bat ^
 --input . ^
 --exclude-input ext ^
 --exclude-input build ^
 --exclude-input bin ^
 --exclude-input objects ^
 --exclude-input projects ^
 --exclude-input ndoc ^
 --exclude-input doc ^
 --output HTML .\doc\reflect-docs\ ^
 --style style ^
 --project .\projects\ndoc\ ^
 %*

popd
