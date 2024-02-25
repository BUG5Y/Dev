#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <zlib.h> // for CRC32

namespace core{
std::string subID[2];

std::string generateID() {
    int length = 40;
    std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string UID;
    for (int i = 0; i < length; ++i) {
        UID += characters[rand() % characters.size()];
    }
    
    // Assuming subOne() and subTwo() are meant to set values in subID
    subOne(); // Set subID[0]
    subTwo(); // Set subID[1]
    
    std::string tempID = UID + subID[2];

    // Calculate CRC32 hash
    uLong crc = crc32(0L, (const Bytef *)tempID.c_str(), tempID.length());

    // Convert CRC32 value to hexadecimal string
    std::stringstream ss;
    ss << std::hex << crc;
    std::string xID = ss.str();

    return xID;
}

void subOne() {
    subID[0] = "test"; // Replace with OS Version
}

void subTwo() {
    subID[1] = "Test"; // Replace with Registry key
}

}
/*
  //�������� NetBIOS.
  int size = sizeof(cid) / sizeof(WCHAR);
  if(CWA(kernel32, GetComputerNameW)(cid, (LPDWORD)&size) == FALSE)CryptedStrings::_getW(CryptedStrings::id_core_botid_unknown, cid);

  //�������� ������. ����� ������ ������� �� ������ Mem::_zero().
  OSVERSIONINFOEXW ovi;
  Mem::_zero(&ovi, sizeof(OSVERSIONINFOEXW));
  ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
  if(CWA(kernel32, GetVersionExW)((OSVERSIONINFOW *)&ovi) == FALSE)Mem::_zero(&ovi, sizeof(OSVERSIONINFOEXW));
  else Mem::_zero(ovi.szCSDVersion, sizeof(ovi.szCSDVersion));

  {
    CSTR_GETW(regKey, core_botid_regkey);

    //���� ���������.
    {
      CSTR_GETW(regValue1, core_botid_regvalue_1);
      subId[0] = Registry::_getValueAsDword(HKEY_LOCAL_MACHINE, regKey, regValue1);
    }

    //������ � �����������.
    {
      CSTR_GETW(regValue2, core_botid_regvalue_2);
      subId[1] = Registry::_getsCrc32OfValue(HKEY_LOCAL_MACHINE, regKey, regValue2);
    }
  }

  //������� ������ ID
  {
    CSTR_GETW(format, core_botid_format);
    size = Str::_sprintfW(buf, 60, format, cid, Crypt::crc32Hash((LPBYTE)&ovi, sizeof(OSVERSIONINFOEXW)), Crypt::crc32Hash((LPBYTE)subId, sizeof(subId)));
  }

  if(size < 1)CryptedStrings::_getW(CryptedStrings::id_core_botid_format_error, buf);
}


void Core::getCurrentBotnetName(LPWSTR name)
{
  LocalSettings::SETTINGS ls;
  LocalSettings::getCurrent(&ls);

  if(ls.flags & LocalSettings::FLAG_BOTNET)
  {
    Str::_CopyW(name, ls.botnet, -1);
  }
  else
  {
    BASECONFIG bs;
    Core::getBaseConfig(&bs);
    Str::_CopyW(name, bs.defaultBotnet, -1);
  }
}
*/