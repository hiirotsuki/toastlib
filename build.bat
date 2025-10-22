cl.exe /c /std:c++20 /EHsc /await /DTOAST_EXPORTS /DWINRT_LEAN_AND_MEAN /I. toastlib.cpp
link.exe /DLL /OUT:toastlib.dll /IMPLIB:toastlib.lib toastlib.obj windowsapp.lib runtimeobject.lib
