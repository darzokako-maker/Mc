#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <fstream>
#include <string>

// --- AYARLAR ---
#define DLL_NAME "bridge.dll"

struct MAPPING_DATA {
    void* pImageBase;
    HMODULE(WINAPI* pLoadLibraryA)(LPCSTR);
    FARPROC(WINAPI* pGetProcAddress)(HMODULE, LPCSTR);
    UINT_PTR pOriginalRip; 
};

// Shellcode fonksiyonu (Senin paylaştığın Manuel Map mantığı - Değişmedi)
void __stdcall Shellcode(MAPPING_DATA* pData); 

// --- YARDIMCI FONKSİYONLAR ---
DWORD GetPID(const char* procName) {
    DWORD pid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32; pe32.dwSize = sizeof(pe32);
        if (Process32First(hSnap, &pe32)) {
            do { if (!_stricmp(pe32.szExeFile, procName)) { pid = pe32.th32ProcessID; break; } } while (Process32Next(hSnap, &pe32));
        }
        CloseHandle(hSnap);
    }
    return pid;
}

DWORD GetThreadID(DWORD pid) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te; te.dwSize = sizeof(te);
    if (Thread32First(hSnap, &te)) {
        do { if (te.th32OwnerProcessID == pid) { CloseHandle(hSnap); return te.th32ThreadID; } } while (Thread32Next(hSnap, &te));
    }
    CloseHandle(hSnap);
    return 0;
}

// --- ANA ENJEKTÖR MANTIĞI ---
int main() {
    system("color 0b"); // Havalı görünmesi için cyan rengi
    printf("--- YAHYA STEALTH INJECTOR V6 ---\n\n");
    
    DWORD pid = 0;
    int choice;
    printf("[1] SonOyuncu'yu Otomatik Bul\n");
    printf("[2] Manuel PID Gir\n");
    printf("\nSeciminiz: ");
    std::cin >> choice;

    if (choice == 1) {
        printf("[*] SonOyuncu bekleniyor...\n");
        // SonOyuncu bazen 'SonOyuncu.exe' bazen de 'javaw.exe' olarak çalışır
        while (!pid) { 
            pid = GetPID("SonOyuncu.exe");
            if(!pid) pid = GetPID("javaw.exe"); 
            Sleep(500); 
        }
    } else {
        printf("[*] Hedef PID numarasini girin: ");
        std::cin >> pid;
    }

    printf("[!] Surec Baglandi! PID: %d\n", pid);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) {
        printf("[X] Surece erisim saglanamadi! (Yonetici olarak calistirin)\n");
        system("pause");
        return 1;
    }

    // --- BURADAN SONRASI SENIN MANUEL MAP MANTIGIN ---
    // 1. DLL Dosyasını Oku
    std::ifstream file(DLL_NAME, std::ios::binary | std::ios::ate);
    if (file.fail()) {
        printf("[X] %s bulunamadi! Klasoru kontrol edin.\n", DLL_NAME);
        CloseHandle(hProc);
        system("pause");
        return 1;
    }

    auto size = file.tellg();
    BYTE* pSrcData = new BYTE[(UINT_PTR)size];
    file.seekg(0, std::ios::beg);
    file.read((char*)pSrcData, size);
    file.close();

    // ... (Senin paylaştığın geri kalan enjeksiyon kodları: VirtualAllocEx, WriteProcessMemory, Thread Hijacking) ...
    // Bu kısım senin önceki mesajında attığın kodun aynısı olacak.

    printf("\n[SUCCESS] Islem tamamlandi! Hile aktif.\n");
    CloseHandle(hProc);
    delete[] pSrcData;
    Sleep(3000);
    return 0;
}
