#include <fstream>
#include <iostream>
#include <string>

//char temp_fn[12][100];//テンプレートのファイル名[文字列][データ番号]
//char input_fn[12][100];//入力データのファイル名[文字列][データ番号]

std::string temp_fname[100];  //テンプレートのファイル名[データ番号]
std::string input_fname[100]; //入力データのファイル名語[データ番号]

std::string temp_word[100];  //テンプレートの語[文字列][データ番号]
std::string input_word[100]; //入力データの語[文字列][データ番号]

int temp_frame[100] = {0};  //テンプレートのフレーム数[フレーム数][データ番号]
int input_frame[100] = {0}; //入力データのフレーム数[フレーム数][データ番号]

double temp_data[130][15][100] = {0.0};  //テンプレートを格納する配列[フレーム][次元][データ番号]
double input_data[130][15][100] = {0.0}; //入力データを格納する配列[フレーム][次元][データ番号]

double route[130][130] = {0}; //DPマッチングの累積距離を格納する[縦フレーム数][横フレーム数]

double reco_rate[100] = {0}; //認識率[入力データ番号]

int reco_number[100] = {0}; //一致したテンプレートの番号を格納

int main()
{

    //テンプレートのデータ入力, city011
    int max_fram=0;
    for (int i = 0; i < 100; i++)
    {
        std::string dir; //番号をcahr型で格納
        //std::cout << ((i+1) / 100) << ", " << (((i+1) % 100) / 10) << ", " << (((i+1) % 100) % 10) << std::endl;
        //std::cout << ((i+1) / 100) + '0' << ", " << (((i+1) % 100) / 10) + '0' << ", " << (((i+1) % 100) % 10) + '0' << std::endl;
        //std::cout << char(((i+1) / 100) + '0') << ", " << char((((i+1) % 100) / 10) + '0') << ", " << char((((i+1) % 100) % 10) + '0') << std::endl;
        dir = char(((i+1) / 100) + '0');        //100の位
        dir += char((((i+1) % 100) / 10) + '0'); //10の位
        dir += char((((i+1) % 100) % 10) + '0'); //1の位
        dir = "./data/city011/city011_" + dir + ".txt";
        //std::cout<< dir << std::endl;
        std::ifstream temp(dir);
        if (temp.fail())
        {
            std::cerr << "Failed to open file." << std::endl;
            return -1;
        }
        temp >> temp_fname[i];
        temp >> temp_word[i];
        temp >> temp_frame[i];
        //if(temp_frame[i] > max_fram) max_fram = temp_frame[i];
        for (int j = 0; j < temp_frame[i]; j++)
        {
            for (int k = 0; k < 15; k++)
            {
                temp >> temp_data[j][k][i];
                //std::cout << temp_data[j][k][i] << ",";
            }
            //std::cout << std::endl;
        }
        temp.close();
        //std::ifstream temp[2]={"./data/city011/city011_001", "./data/city011/city011_002"};
    }

    //入力データ入力, city021
    for (int i = 0; i < 100; i++)
    {
        std::string dir; //番号をstring型で格納
        dir = char(((i+1) / 100) + '0');        //100の位
        dir += char((((i+1) % 100) / 10) + '0'); //10の位
        dir += char((((i+1) % 100) % 10) + '0'); //1の位
        dir = "./data/city021/city021_" + dir + ".txt";
        //std::cout<< dir << std::endl;
        std::ifstream input(dir);
        if (input.fail())
        {
            std::cerr << "Failed to open file." << std::endl;
            return -1;
        }
        input >> input_fname[i];
        input >> input_word[i];
        input >> input_frame[i];
        //if(temp_frame[i] > max_fram) max_fram = temp_frame[i];
        for (int j = 0; j < input_frame[i]; j++)
        {
            for (int k = 0; k < 15; k++)
            {
                input >> input_data[j][k][i];
                //std::cout << input_data[j][k][i] << ",";
            }
            //std::cout << std::endl;
        }
        input.close();
        //std::ifstream temp[2]={"./data/city011/city011_001", "./data/city011/city011_002"};
    }

    //std::cout << max_fram << std::endl;
    return 0;
}