
#include <iostream>

#include "DCM.h"



int main()
{
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

