#include "aescriptsLicensing.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
    
using namespace std;

int main(int argc, char* argv[])
{
    aescripts::LicenseData lData;
    lData.nofUsers = 0;
    string productName = "myproduct"; // change this to your own product!
    string productID = "AESLT"; // change this to your own productID!
    int privNum = 112233; // change this to your own privNum!
    int result = aescripts::loadLicenseFromFile(productName.c_str(), productID.c_str(), privNum, lData, false);
    string status = "invalid";
    if (result == 0) status = "valid";
    cout << "status: " << status << " license\n";
    if (result == 0) {
        cout << "first name: '" << lData.firstName << "'\n";
        cout << "last name: '" << lData.lastName << "'\n";
        cout << "number of user licenses: " << lData.nofUsers << "\n";
        cout << "license type: '" << lData.licType << "'\n";
        cout << "product ID: '" << lData.productID << "'\n";
        cout << "product version: '" << lData.productVersion << "'\n";
        cout << "serial: '" << lData.serial << "'\n";
    }
    if (result == -7) {
        cout << "trial mode, days since trial start: " << lData.numberOfDaysSinceFirstStart << "\n";
    }
    return 0;
}
    
