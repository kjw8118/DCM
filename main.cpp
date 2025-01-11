
#include <iostream>

#include "DCM.h"

#include <Windows.h>
#include <iomanip>

bool isValidEucKr(unsigned char byte1, unsigned char byte2) {
    char eucKrBytes[3] = { static_cast<char>(byte1), static_cast<char>(byte2), '\0' };
    wchar_t wideChar[2] = { 0 };

    // EUC-KR -> WideChar 변환
    if (MultiByteToWideChar(949, MB_ERR_INVALID_CHARS, eucKrBytes, -1, wideChar, 2) == 0)
        return false;

    wchar_t wc = wideChar[0];
    if ((wc >= 0xAC00 && wc <= 0xd7a3) ||
        (wc >= 0x3000 && wc <= 0x303f))
        return true;

    return false;
}

int main()
{
    
    char mix[] = { 0xc3, 0xbc, 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x20, 0x00 };
    char newMix[] = { 0xFC, 0xC7, 0xD1, 0xB1, 0xDB, 0x00 };

    SetConsoleOutputCP(CP_UTF8);
    std::string mix_str(mix);
    std::cout << mix_str << std::endl;
    /*for (unsigned char c : mix_str)
        std::cout << (int)c << ", ";
    std::cout << std::endl;
    
    SetConsoleOutputCP(51949);
    auto mix_str_euckr = GIT::utf8ToLocal(mix_str);
    std::cout << mix_str_euckr << std::endl;
    for (unsigned char c : mix_str_euckr)
        std::cout << (int)c << ", ";
    std::cout << std::endl;

    SetConsoleOutputCP(28591);
    auto mix_str_latin1 = GIT::utf8ToLatin1(mix_str);
    std::cout << mix_str_latin1 << std::endl;
    for (unsigned char c : mix_str_latin1)
        std::cout << (int)c << ", ";
    std::cout << std::endl;*/

    SetConsoleOutputCP(CP_UTF8);
    std::string newMix_str(newMix);    
    std::cout << newMix_str << std::endl;
    for (unsigned char c : newMix_str)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;
    
    auto newMix_str_utf8 = GIT::mixedToUtf8(newMix);
    std::cout << newMix_str_utf8 << std::endl;
    for (unsigned char c : newMix_str_utf8)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;

    auto newMix_str_euckr = GIT::utf8ToEucKrAndLatin1(newMix_str_utf8);
    std::cout << newMix_str_euckr << std::endl;
    for (unsigned char c : newMix_str_euckr)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;

    return 0;

    char c = 252;
    std::string s1 = "";
    char u = 'ü';
    std::string u1 = "";
    u1.push_back(u);
    s1.push_back(c);
    std::string s2 = GIT::localToUtf8(s1);
    std::string s3 = GIT::utf8ToLocal(s1);
    std::string u2 = GIT::localToUtf8(u1);
    std::string u3 = GIT::utf8ToLocal(u1);
    std::string h1 = "한글";
    std::cout << h1 << std::endl;
    for (unsigned char cc : h1)
        std::cout << (int)(cc) << ", ";
    std::cout << std::endl;
    std::string h2 = GIT::localToUtf8(h1);
    std::cout << h2 << std::endl;
    for (unsigned char cc : h2)
        std::cout << (int)(cc) << ", ";
    std::cout << std::endl;
    std::string h3 = GIT::utf8ToLocal(h2);
    std::cout << h3 << std::endl;
    for (unsigned char cc : h3)
        std::cout << (int)(cc) << ", ";
    std::cout << std::endl;
    std::string h4 = GIT::utf8ToLatin1(h2);
    std::cout << h4 << std::endl;
    for (unsigned char cc : h4)
        std::cout << (int)(cc) << ", ";
    std::cout << std::endl;
    std::string h5 = GIT::latin1ToUtf8(h4);
    std::cout << h5 << std::endl;
    for (unsigned char cc : h5)
        std::cout << (int)(cc) << ", ";
    std::cout << std::endl;
    std::cout << c << std::endl;
    return 0;
    auto manager = new DCM::Manager();
    manager->openWithRepo("repo\\Test_DCM2.dcm", "Jinwon", "kjw8118@gmail.com");
    //manager->createDCM();
    manager->parse();
    //manager->saveAsDCM("repo\\Test_DCM2.dcm");
    auto element = (DCM::GroupCharMap*)manager->findElement("Two_D_group", true);
    
    manager->removeElement(19);
    manager->removeElement(24);
    
    std::transform(element->values.begin(), element->values.end(), element->values.begin(), [&](const double& value)->double {return value * 2; });
    auto diffResults = manager->getDiffWithCurrent();
    /*for (auto& diffResult : diffResults)
    {
        for (auto &hunk : diffResult.diffHunks)
        {
            auto oldStart = hunk.hunk.old_start;
            auto oldLine = hunk.hunk.old_lines;
            auto newStart = hunk.hunk.new_start;
            auto newLine = hunk.hunk.new_lines;

            std::u8string old_text, new_text;
            for (auto line : hunk.diffLines)
            {
                switch (line.type)
                {
                case line.CONTEXT:
                    new_text += line.line;
                    old_text += line.line;
                    break;
                case line.ADDED:                    
                    new_text += line.line;
                    break;
                case line.DELETED:
                    old_text += line.line;
                    break;
                }
            }

            std::cout << "Old Text" << std::endl;
            std::cout << GIT::u8utf8ToLocal(old_text) << std::endl;
            std::cout << "New Text" << std::endl;
            std::cout << GIT::u8utf8ToLocal(new_text) << std::endl;
        }
    }
    auto diff = manager->getDiffWithCurrent();*/
    auto rebuildString = manager->rebuildElements();
    std::cout << rebuildString;
    //manager->saveAsDCM("repo\\Test_DCM2.dcm");
    //std::cout << "Hello World!\n";
    
    //std::cout << "DCM Parser General Test\t" << (bool)DCM::Parser::test() << std::endl;
    
    /*auto manager = new DCM::Manager();
    manager->open("Test_DCM2.dcm");
    manager->parse();
    std::cout << manager->rebuild() << std::endl;

    //DCM::Parser::rebuildArrayTest();
    //DCM::Parser::rebuildMatrixTest();
    //DCM::Parser::rebuildLineBaseParameterTest();
    //DCM::Parser::rebuildMapBaseParameterTest();
    //DCM::Parser::rebuildDistributionTest();
    //DCM::Parser::createDCMTest();*/
    
    return 0;

}

