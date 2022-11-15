#include "../shared.h"
#include <iostream>
#include <jni.h>
#include <vector>
#include <fstream>
#include <windows.h>

HMODULE module;

/**
* Get the exact path of our DLL
* 
* @return Exact path of this DLL (i.e C:\test\lib.dll)
*/
char* get_dll_path() {
    char path[MAX_PATH];
    if (GetModuleFileNameA(module, path, sizeof(path)) == 0)
    {
        return nullptr;
    }
    return path;
}


DWORD WINAPI THREAD(LPVOID _) {
#ifdef VERBOSE
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    AttachConsole(GetCurrentProcessId());
    SetConsoleTitleA("Debug");
#endif

    // Fetch JVM from process (should only be 1 JVM?)
    JavaVM* jvm = new JavaVM[1];
    jint res = JNI_GetCreatedJavaVMs(&jvm, 1, nullptr);
    JNIEnv* env = nullptr;

    // Setup JVM environment
    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_8) == JNI_EDETACHED) {
        jvm->AttachCurrentThreadAsDaemon((void**)&env, NULL);
    }

    // If JVM environment was setup:
    if (env != nullptr) {
        // Fetch the system class loader with the static Java function ClassLoader#getSystemClassLoader()
        auto class_loader = env->FindClass("java/lang/ClassLoader");
        auto get_system_loader =
            env->GetStaticMethodID(class_loader, "getSystemClassLoader",
                "()Ljava/lang/ClassLoader;");
        auto system_loader =
            env->CallStaticObjectMethod(class_loader, get_system_loader);

        // Fetch the binary of the Run Java class
        std::string class_path;
        std::string dir = get_dir(get_dll_path());
        (class_path += dir) += "\\lib\\Run.class";
        printf("Dir: %s", dir.c_str());
        std::ifstream fl(class_path.c_str(), std::ios::binary | std::ios::ate); // Make sure this class is compatible with the JVM version
        fl.seekg(0, std::ios::end);
        size_t len = fl.tellg();
        std::vector<char> buffer(len);
        fl.seekg(0, std::ios::beg);
        fl.read(buffer.data(), len);
        fl.close();

        // If a class named Run already exists, maybe we've already injected?
        if (env->FindClass("Run") && !env->ExceptionCheck()) {
            printf("Already injected?\n");
            goto end; // If we define a class that already exists, JVM will crash
        }

        env->ExceptionClear();

        // Define Run class with fetched binary
        printf("data: %s", buffer.data());
        jclass clazz = env->DefineClass("Run", system_loader, reinterpret_cast<jbyte*>(buffer.data()), len);

        // Run static Java method from Run class Run#run(String path)
        auto run_method = env->GetStaticMethodID(clazz, "run", "(Ljava/lang/String;)V");
        env->CallStaticVoidMethod(clazz, run_method, env->NewStringUTF(dir.c_str()));
        printf("%d\n", run_method);
#ifdef VERBOSE
        if (env->ExceptionCheck())
        {
            env->ExceptionDescribe();
        }
#endif
        
    }

    // Cleanup
end:
    jvm->DetachCurrentThread();
#ifdef VERBOSE
    FreeConsole();
#endif
    FreeLibraryAndExitThread(module, 0);
    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            // On attach, cache HMODULE and create new thread that calls THREAD function (main logic)
            module = hModule;
            CreateThread(NULL, 0, THREAD, 0, 0, NULL);
            // We don't listen for any other calls
            DisableThreadLibraryCalls(module);
    }
    return TRUE;
}
