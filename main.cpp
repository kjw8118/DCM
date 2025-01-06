
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
    std::transform(element->values.begin(), element->values.end(), element->values.begin(), [&](const double& value)->double {return value * 2; });
    manager->getDiffWithCurrent();
    manager->saveAsDCM("repo\\Test_DCM2.dcm");
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

