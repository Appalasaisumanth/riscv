#include <iostream>
#include <regex>
#include <string>
#include <bitset>
#include <stdexcept>  //library functions
#include <cmath>
#include <emscripten.h>
#include <vector>
#include <map>
using namespace std;
vector<string> Instruction; //Instruction memory
map<string, string> to_type;//mapping for assembler converting assembly to machine code
map<string, int> label;//mapping for labels and indexes
map<string, string> Reg; //mapping for registers
int cycle=0;
void fetch(int cycle);//fetch of pipeline
void decode(int cycle); 
void execute(int cycle);
void memory(int cycle);
void write_Back(int cycle);
vector<int> GPR;
vector<int> GPR_VALID;
vector<int> MEMORY;
bool FLAG = false;
bool rev = false;
int stall = 0;
int flash = 0;
int cycles = 0;
string FETCH_INS = "";
string DECODE_INS = "";
string EXECUTE_INS = "";
string MEMORY_INS = "";
string WRITE_BACK_INS = "";
int pc = 0;
int ifid_ir = -1;
int ifid_npc = -1;
int ifid_dpc = -1;
int idex_pc = -1;
int idex_imm_upper = -1;
int idex_control = -1;
int idex_reg1 = -1;
int idex_reg2 = -1;
int idex_imm_lower = -1;
int idex_function3 = -1;
int idex_regd = -1;
int exmo_control = -1;
int exmo_aluout = -1;
int exmo_reg2 = -1;
int exmo_regd = -1;
int mowb_control = -1;
int mowb_aluout = -1;
int mowb_ldout = -1;
int mowb_regd = -1;
bool s1 = false;
bool s2 = false;
bool FLAG2 = true;
string number_to_binary(long long num, int len)
{

    // Convert the number to a 5-bit binary string
    bitset<32> binary(num);
    string bin_string = binary.to_string();
    return bin_string.substr(32 - len, len);
}
void create_map()
{
    to_type["add"] = "R0000000rs2rs1000rd0110011"; // R-type instructions
    to_type["sub"] = "R0100000rs2rs1000rd0110011";
    to_type["xor"] = "R0000000rs2rs1100rd0110011";
    to_type["or"] = "R0000000rs2rs1110rd0110011";
    to_type["and"] = "R0000000rs2rs1111rd0110011";

    to_type["sll"] = "R0000000rs2rs1001rd0110011";
    to_type["srl"] = "R0000000rs2rs1101rd0110011";
    to_type["sra"] = "R0100000rs2rs1101rd0110011";

    to_type["slt"] = "R0000000rs2rs1010rd0110011";
    to_type["sltu"] = "R0000000rs2rs1011rd0110011";

    to_type["addi"] = "Iimmrs1000rd0010011";
    to_type["xori"] = "Iimmrs1100rd0010011";
    to_type["ori"] = "Iimmrs1110rd0010011"; // i type instructions
    to_type["andi"] = "Iimmrs1111rd0010011";
    to_type["slti"] = "Iimmrs1010rd0010011";
    to_type["sltiu"] = "Iimmrs1011rd0010011";

    to_type["slli"] = "Himmrs1001rd0010011"; // #i1 only 5 bits imm
    to_type["srli"] = "Himmrs1101rd0010011"; // i1
    to_type["srai"] = "Himmrs1101rd0010011"; // i1 its set is H for logical shits

    to_type["lb"] = "Limmrs1000rd0000011";
    to_type["lh"] = "Limmrs1001rd0000011";
    to_type["lw"] = "Limmrs1010rd0000011";
    to_type["lbu"] = "Limmrs1100rd0000011";
    to_type["lhu"] = "Limmrs1011rd0010011";
    to_type["jalr"] = "Iimmrs1000rd1100111";

    to_type["sb"] = "Simm2rs2rs1000imm10100011"; // divide imm into two parts of 4,8 bits
    to_type["sh"] = "Simm2rs2rs1001imm10100011"; // store
    to_type["sw"] = "Simm2rs2rs1010imm10100011";

    // branch instructions
    to_type["beq"] = "Bimm2rs2rs1000imm11100011"; // imm4:12th bit imm3:10-5 bits imm2:i-4 imm1:11th bit
    to_type["bne"] = "Bimm2rs2rs1001imm11100011";
    to_type["blt"] = "Bimm2rs2rs1100imm11100011"; // branch
    to_type["bge"] = "Bimm2rs2rs1101imm11100011";
    to_type["bltu"] = "Bimm2rs2rs1110imm11100011";
    to_type["bgeu"] = "Bimm2rs2rs1111imm11100011";
    // jump instruction
    to_type["jal"] = "Jimmrd1101111"; // 20 imm bits imm4:20 imm3:10-1 imm2:11 imm1:19-12
    // upper instruction
    to_type["lui"] = "Uimmrd0110111"; // upper
    // ecall i type system calls
    to_type["ecall"] = "Kimmrs1000rd1110011";  // put 0 in imm transfer control to os
    to_type["ebreak"] = "Eimmrs1000rd1110011"; // put 1 in imm transfer control to debugger
    // extensions

    to_type["mul"] = "R0000001rs2rs1000rd0110011";
    to_type["mulh"] = "R0000001rs2rs1001rd0110011";
    to_type["mulsu"] = "R0000001rs2rs1010rd0110011";
    to_type["mulu"] = "R0000001rs2rs1011rd0110011";
    to_type["div"] = "R0000001rs2rs1100rd0110011";
    to_type["divu"] = "R0000001rs2rs1101rd0110011";
    to_type["rem"] = "R0000001rs2rs1110rd0110011";
    to_type["remu"] = "R0000001rs2rs1111rd0110011";

    // cout<<to_type["lb"]<<endl;
    // cout <<number_to_binary(4096,20) << endl;
     //based on types of 32 registers
    Reg["ra"]="x1";
    Reg["sp"]="x2";
    Reg["gp"]="x3";
      Reg["tp"]="x4";
    Reg["t0"]="x5";
    Reg["t1"]="x6";
     Reg["t2"]="x7";
    Reg["s0"]="x8";
    Reg["fp"]="x8";
      Reg["s1"]="x9";
    Reg["a0"]="x10";
    Reg["a1"]="x11";
     Reg["a2"]="x12";
    Reg["a3"]="x13";
    Reg["a4"]="x14";
      Reg["a5"]="x15";
    Reg["a6"]="x16";
    Reg["a7"]="x17";
     Reg["s2"]="x18";
    Reg["s3"]="x19";
    Reg["s4"]="x20";
      Reg["s5"]="x21";
    Reg["s6"]="x22";
    Reg["s7"]="x23";
      Reg["s8"]="x24";
    Reg["s9"]="x25";
    Reg["s10"]="x26";
      Reg["s11"]="x27";
     Reg["t3"]="x28";
     Reg["t4"]="x29"; 
     Reg["t5"]="x30";
     Reg["t6"]="x31";
}
void create_stages()
{

    int i = 0;
    for (i = 0; i < 32; i++)
    {
        GPR.push_back(0);
        GPR_VALID.push_back(0);
    }
    i = 0;
    for (i = 0; i < 1024; i++)
    {
        MEMORY.push_back(-1);
    }
    GPR[0] = 0;
}
string parse_input(string test)
{
    int l = test.length();
    int start = 0;
    string temp = "";
    string array[3];
    while (start < l)

    {
        if (test[start] == ' ')
        {
            start += 1;
            break;
        }
        else if (test[start] == '\n')
        {
            break;
        }
        else
        {
            temp = temp + test[start];
        }

        start += 1;
    }
    if (to_type.find(temp) == to_type.end())
    {
        cout << "error no such instruction2" << endl;
        return "ERROR";
    }
    string ans = to_type[temp];

    if (ans[0] == 'R')
    {
        int count = 0;
        while (start < l)

        {
            if (test[start] == '\n')
            {
                break;
            }
            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            array[count] += test[start];

            start += 1;
        }

        for (int i = 0; i < 3; i++)
        {  
            if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        {   string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
            
        }
        }
        string rd = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string rs2 = number_to_binary(stoi(array[2]), 5);
        regex reg_rs1("rs1");
        regex reg_rs2("rs2");
        regex reg_rd("rd");
        ans = regex_replace(ans, reg_rs1, rs1); // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, rs2); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rd);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }
    else if (ans[0] == 'I')
    {
        int count = 0;
        while (start < l)

        {
            if (test[start] == '\n')
            {
                break;
            }
            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            array[count] += test[start];

            start += 1;
        }
        if(count!=2)
        {
            return "Error format";
        }
       for (int i = 0; i < 2; i++)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        {   if(Reg.find(array[i]) == Reg.end())
        {
            return "error of register";
        }
             string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
      
        string rd = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string imm = number_to_binary(stoi(array[2]), 12);
       
        regex reg_rs1("rs1");
        regex reg_rs2("imm");
        regex reg_rd("rd");
        ans = regex_replace(ans, reg_rs1, rs1); // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rd);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }

    else if (ans[0] == 'H') // h for some special immediate shifts
    {
        int count = 0;
        while (start < l)

        {
            if (test[start] == '\n')
            {
                break;
            }
            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            array[count] += test[start];

            start += 1;
        }
  if(count!=1)
        {
            return "Error format";
        }
        for (int i = 0; i < 2; i++)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        { if(Reg.find(array[i]) == Reg.end())
        {
            return "error of register";
        }
            string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
     
        string rd = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string imm = number_to_binary(stoi(array[2]), 5);
        if (temp == "srai")
        {
            imm = "0100000" + imm;
        }
        else
        {
            imm = "0000000" + imm;
        }
        
        regex reg_rs1("rs1");
        regex reg_rs2("imm");
        regex reg_rd("rd");
        ans = regex_replace(ans, reg_rs1, rs1); // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rd);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }
    else if (ans[0] == 'L') // l for load
    {

        int count = 0;
        while (start < l)

        {

            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            if (test[start] == '(')
            {
                count += 1;
                start += 1;
            }
            if (test[start] == ')')
            { 
                break;
            }
            array[count] += test[start];

            start += 1;
        }

        // array[0] = array[0].substr(1, array[0].length() - 1);
        // array[2] = array[2].substr(1, array[2].length() - 1);
          if(count!=2)
        {
            return "Error format";
        }
 for (int i = 0; i < 3; i+=2)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        {      string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
        string var = "tempo";
        var = array[2];
        array[2] = array[1];
        array[1] = var;

        string rd = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string imm = number_to_binary(stoi(array[2]), 12);

       
        regex reg_rs1("rs1");
        regex reg_rs2("imm");
        regex reg_rd("rd");
        ans = regex_replace(ans, reg_rs1, rs1); // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm);
        ans = regex_replace(ans, reg_rd, rd);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }
    else if (ans[0] == 'S') // s for store instruction2
    {

        int count = 0;
        while (start < l)

        {

            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            if (test[start] == '(')
            {
                count += 1;
                start += 1;
            }
            if (test[start] == ')')
            {
                break;
            }
            array[count] += test[start];

            start += 1;
        }
  if(count!=2)
        {
            return "Error format";
        }
        // array[0] = array[0].substr(1, array[0].length() - 1);
        // array[2] = array[2].substr(1, array[2].length() - 1);
         for (int i = 0; i < 3; i+=2)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        { if(Reg.find(array[i]) == Reg.end())
        {
            return "error of register";
        }
            string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
        string var = "tempo";
        var = array[2];
        array[2] = array[1];
        array[1] = var;

        string rs2 = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string imm = number_to_binary(stoi(array[2]), 12);
        var = "";
        for (int i = 0; i < 5; i++)
        {
            var += imm[i];
        }
        string imm1 = var;
        string imm2 = imm.substr(5);
     
        regex reg_rs1("rs1");
        regex reg_rs2("imm1");
        regex reg_rd("rs2");
        regex reg_imm2("imm2");
        ans = regex_replace(ans, reg_rs1, rs1);  // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm1); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rs2);
        ans = regex_replace(ans, reg_imm2, imm2);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }
    else if (ans[0] == 'B')
    {

        int count = 0;
        while (start < l)

        {
            if (test[start] == '\n')
            {
                break;
            }
            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }

            array[count] += test[start];

            start += 1;
        }
         

        // array[0] = array[0].substr(1, array[0].length() - 1);
        // array[1] = array[1].substr(1, array[1].length() - 1);
         for (int i = 0; i < 2; i++)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        { if(Reg.find(array[i]) == Reg.end())
        {
            return "error of register";
        }
             string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
        
        string rs2 = number_to_binary(stoi(array[0]), 5);
        string rs1 = number_to_binary(stoi(array[1]), 5);
        string imm = number_to_binary(stoi(array[2])>>1, 12);
    

        // Extract and concatenate characters correctly
        string imm2 = std::string(1, imm[0]) + std::string(1, imm[2]) + std::string(1, imm[3]) + std::string(1, imm[4]) + std::string(1, imm[5]) + std::string(1, imm[6]) + std::string(1, imm[7]);

        string imm1 = std::string(1, imm[8]) + std::string(1, imm[9]) + std::string(1, imm[10]) + std::string(1, imm[11]) + std::string(1, imm[1]);

        regex reg_rs1("rs1");
        regex reg_rs2("imm1");
        regex reg_rd("rs2");
        regex reg_imm2("imm2");
        ans = regex_replace(ans, reg_rs1, rs1);  // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm1); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rs2);
        ans = regex_replace(ans, reg_imm2, imm2);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }
    else if (ans[0] == 'U')
    {int count = 0;
        while (start < l)
        {
            if (test[start] == '\n')
            {
                break;
            }
            if (test[start] == ',')
            {
                count += 1;
                start += 1;
            }
            array[count] += test[start];

            start += 1;
        }
        
        for (int i = 0; i < 1; i++)
        {  if(array[i][0]=='x'){
            array[i] = array[i].substr(1, array[i].length() - 1);
        }
        else
        { if(Reg.find(array[i]) == Reg.end())
        {
            return "error of register";
        }
             string temp_s=Reg[array[i]];
            array[i]=temp_s.erase(0,1);
        }
        }
      
        string rd = number_to_binary(stoi(array[0]), 5);
        string imm = number_to_binary(stoi(array[1]), 20);
        regex reg_rs2("imm");
        regex reg_rd("rd");
        // rs1 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rs2, imm); // rs2 replaced with its 5-bit binary string
        ans = regex_replace(ans, reg_rd, rd);
        ans += '\n';
        ans = ans.substr(1, ans.length() - 1);
        return ans;
    }

    return "ERROR ";
}
bool check(string test)
{
    int t = test.length();
    for (int i = 0; i < t; i++)
    {
        if (test[i] == ':')
        {
            return true;
        }
    }
    return false;
}
string Modify_string(string test)
{ string temp="";
int count=0;
bool bool_char_now=false;
bool bool_space_now=false;

     for(size_t i=0;i<=test.length()-1;i++)
{
   test[i]=std::tolower(test[i]);
   if (test[i]!=' ' )
   {
    temp=temp+test[i];
    bool_char_now=!(bool_space_now);
   }
   if (test[i]==' ' && bool_char_now)
   {
    temp+=test[i];
    bool_space_now=true;
    bool_char_now=false;
   }
}
  //  cout<<temp<<endl;
    return temp;
}
pair<bool, int> check_label(string &test)
{
    string temp = "";

    for (char c : test)
    {
        if (c == ',')
        {
            temp = "";
        }
        else
        {
            temp += c;
        }
    }

    auto it = label.find(temp);
    if (it != label.end())
    {
        return {true, it->second};
    }
    else
    {
        return {false, 0};
    }
}
string parse_text(string test)
{
    string testing[100];
    int l = test.length();
    int i = 0, index = 0;

    while (i < l)
    {
        if (test[i] == '\n')
        {
            index += 1;
        }
        else
        {
            testing[index] = testing[index] + test[i];
        }
        i += 1;
    }
    int count = 0;
    for (int i = 0; i <= index; i++)
    {
        int temp = testing[i].length();
        int k = 0;
        while (k < temp)
        {
            if (testing[i][k] == ':')
            {
                int t = testing[i].length();
                string tem = testing[i];
                tem.erase(tem.end() - 1, tem.end());
                label[tem] = (i - count);
                count += 1;
            }
            k++;
        }
    }
    string testing2[1000];
    int count2 = 0;
    for (int i = 0; i <= index; i++)
    {
        if (!(check(testing[i])))
        {
            testing2[count2] = testing[i];
            count2++;
        }
    }

    for (i = 0; i < count2 - 1; i++)
    {
        pair<bool, int> answer = check_label(testing2[i]);

        if (answer.first)
        {
            int j = 0;
            for (j = testing2[i].length(); j > 0; j--)
            {
                if (testing2[i][j] == ',')
                {
                    break;
                }
            }
            testing2[i] = testing2[i].substr(0, j + 1);
            int temp = -i + answer.second;
testing2[i]=Modify_string(testing2[i]);
            Instruction.push_back(parse_input(testing2[i] + to_string(temp * 4)).substr(0, 32));
        }
        else
        {testing2[i]=Modify_string(testing2[i]);
            Instruction.push_back(parse_input(testing2[i]).substr(0, 32));
        }
    }

    return "hello";
}
void print_state()
{
    cout << "Total no.of stalls: " << stall << endl;
    cout << "Total no.of flushes: " << flash << endl;
    cout << "----------------------Registers--------------------------------" << endl;
    for (int i = 0; i < 32; i++)
    {
        cout << "Register " << i << ':' << " " << GPR[i] << endl;
    }
    cout << "----------------------------------------------------------------" << endl;

    cout << "               pc: " << pc << endl;
    cout << "----------------------IFID--------------------------------" << endl;
    cout << "          ifid_ir: " << ifid_ir << endl;
    cout << "         ifid_npc: " << ifid_npc << endl;
    cout << "         ifid_dpc: " << ifid_dpc << endl;

    cout << "----------------------IDEX--------------------------------" << endl;
    cout << "          idex_pc: " << idex_pc << endl;
    cout << "   idex_imm_upper: " << idex_imm_upper << endl;
    cout << "     idex_control: " << idex_control << endl;
    cout << "   idex_Register1: " << idex_reg1 << endl;
    cout << "   idex_Register2: " << idex_reg2 << endl;
    cout << "   idex_imm_lower: " << idex_imm_lower << endl;
    cout << "   idex_function3: " << idex_function3 << endl;
    cout << "  idex_Register_d: " << idex_regd << endl;

    cout << "----------------------EXMO--------------------------------" << endl;
    cout << "     exmo_control: " << exmo_control << endl;
    cout << "      exmo_aluout: " << exmo_aluout << endl;
    cout << "   exmo_Register2: " << exmo_reg2 << endl;
    cout << "  exmo_Register_d: " << exmo_regd << endl;

    cout << "----------------------MOWB--------------------------------" << endl;
    cout << "     mowb_control: " << mowb_control << endl;
    cout << "      mowb_aluout: " << mowb_aluout << endl;
    cout << "       mowb_ldout: " << mowb_ldout << endl;
    cout << "  mowb_Register_d:" << mowb_regd << endl;
    cout << "----------------------------------------------------------------" << endl;
}
string Instruction_Decoder(string test)
{ 
  string opcode=test.substr(25,7);
 
  if(opcode=="0110011")
  {
    string func7=test.substr(0,7);
    int rs2=stoi(test.substr(7,5), nullptr, 2);
   int rs1=stoi(test.substr(12,5), nullptr, 2);
    string func3=test.substr(17,3);
    int rd=stoi(test.substr(20,5), nullptr, 2);
    string ans="";
    if(func7[1]=='1')
    {
        if(func3=="000")
        {
            ans+="sub ";
        }
        else if(func3=="101")
        {
            ans+="sra ";
        }
    }
    else if(func7[6]==1)
    {
         if(func3=="000")
        {
            ans+="mul ";
        }
        else if(func3=="001")
        {
            ans+="mulh  ";
        } 
        else if(func3=="010")
        {
            ans+="mulsu ";
        }
        else if(func3=="011")
        {
            ans+="mulu ";
        } 
        else if(func3=="100")
        {
            ans+="div ";
        }
        else if(func3=="101")
        {
            ans+="divu ";
        } 
        else if(func3=="110")
        {
            ans+="rem ";
        }
        else if(func3=="111")
        {
            ans+="remu ";
        }
    }
    else 
    {
         if(func3=="000")
        {
            ans+="add ";
        }
        else if(func3=="001")
        {
            ans+="sll  ";
        } 
        else if(func3=="010")
        {
            ans+="slt ";
        }
        else if(func3=="011")
        {
            ans+="sltu ";
        } 
        else if(func3=="100")
        {
            ans+="xor ";
        }
        else if(func3=="101")
        {
            ans+="srl ";
        } 
        else if(func3=="110")
        {
            ans+="or ";
        }
        else if(func3=="111")
        {
            ans+="and ";
        }
       
    }
     ans+='x'+to_string(rd)+",x"+to_string(rs1)+",x"+to_string(rs2);
        return ans;

  }
   else if(opcode=="0010011")
  {
      string IMM=test.substr(0,12);
      int imm= (IMM[0]=='0')?(stoi(IMM, nullptr, 2)):(stoi(IMM, nullptr, 2)-4096);
   int rs1=stoi(test.substr(12,5), nullptr, 2);
    string func3=test.substr(17,3);
    int rd=stoi(test.substr(20,5), nullptr, 2);
    string ans="";
     if(func3=="000")
        {
            ans+="addi ";
        }
        else if(func3=="001")
        {
            ans+="slli  ";
        } 
        else if(func3=="010")
        {
            ans+="slti ";
        }
        else if(func3=="011")
        {
            ans+="sltui ";
        } 
        else if(func3=="100")
        {
            ans+="xori ";
        }
      
         else if(func3=="101" && IMM[1]=='1')
        {
            ans+="srai ";
        } 
           else if(func3=="101")
        {
            ans+="srli ";
        }
        else if(func3=="110")
        {
            ans+="ori ";
        }
        else if(func3=="111")
        {
            ans+="andi ";
        }
         ans+='x'+to_string(rd)+",x"+to_string(rs1)+','+to_string(imm);
         return ans;
  }
     else if(opcode=="0000011")
  {
      string IMM=test.substr(0,12);
      int imm= (IMM[0]=='0')?(stoi(IMM, nullptr, 2)):(stoi(IMM, nullptr, 2)-4096);
   int rs1=stoi(test.substr(12,5), nullptr, 2);
    string func3=test.substr(17,3);
    int rd=stoi(test.substr(20,5), nullptr, 2);
    string ans="";
     if(func3=="000")
        {
            ans+="lb ";
        }
        else if(func3=="001")
        {
            ans+="lh  ";
        } 
        else if(func3=="010")
        {
            ans+="lw ";
        }
     
        else if(func3=="100")
        {
            ans+="lbu ";
        }
      
           else if(func3=="101")
        {
            ans+="lhu ";
        }
         ans+='x'+to_string(rd)+','+to_string(imm)+"(x"+to_string(rs1)+')';
         return ans;
  }
  else if( opcode=="0100011")
  {
    string IMM=test.substr(0,7)+test.substr(20,5);
     int imm= (IMM[0]=='0')?(stoi(IMM, nullptr, 2)):(stoi(IMM, nullptr, 2)-4096);
    int rs2=stoi(test.substr(7,5), nullptr, 2);
   int rs1=stoi(test.substr(12,5), nullptr, 2);
    string func3=test.substr(17,3);

    
    string ans="";
      if(func3=="000")
        {
            ans+="sb ";
        }
        else if(func3=="001")
        {
            ans+="sh  ";
        } 
        else if(func3=="010")
        {
            ans+="sw ";
        }
        ans+='x'+to_string(rs2)+','+to_string(imm)+"(x"+to_string(rs1)+')';
        return ans;
     
  }
    else if( opcode=="1100011")
  {
    string IMM=test.substr(0, 1) + test.substr(24, 1) + test.substr(1, 6) + test.substr(20, 3);
     int imm= (IMM[0]=='0')?(stoi(IMM, nullptr, 2)):(stoi(IMM, nullptr, 2)-2048);
    int rs2=stoi(test.substr(7,5), nullptr, 2);
   int rs1=stoi(test.substr(12,5), nullptr, 2);
    string func3=test.substr(17,3);

    
    string ans="";
      if(func3=="000")
        {
            ans+="beq ";
        }
        else if(func3=="001")
        {
            ans+="bne  ";
        } 
        else if(func3=="100")
        {
            ans+="blt ";
        }
           else if(func3=="101")
        {
            ans+="bge  ";
        } 
        else if(func3=="110")
        {
            ans+="bltu ";
        }
          else if(func3=="111")
        {
            ans+="bgeu ";
        }
        ans+='x'+to_string(rs1)+",x"+to_string(rs2)+','+to_string(imm*4);
        return ans;

     
  }
  else if( opcode=="0110111")
  {
    string IMM=test.substr(0, 20) ;
     int imm= (IMM[0]=='0')?(stoi(IMM, nullptr, 2)):(stoi(IMM, nullptr, 2)-2048);
    int rs2=stoi(test.substr(20,5), nullptr, 2);

    string ans="";
     ans+="lui ";
        ans+='x'+to_string(rs2)+","+to_string(imm);
        return ans;
        
     
  }
  
return test;
}
void fetch(int cycle)
{ // cout<<2<<endl;
    if (pc >= Instruction.size())
    {
        FLAG = true;
        return;
    }
    else
    {

        ifid_ir = pc;
        ifid_dpc = pc;
        ifid_npc = pc + 1;
        FETCH_INS=Instruction[pc];
         cout<<"cycle"<<cycle<<"        FETCH       " <<Instruction_Decoder(FETCH_INS)<<endl;
    }

    if (!rev)
    {
        decode(cycle);
    }
}
void decode(int cycle)
{

    // cout<<3<<endl;
    if (ifid_ir >= 0)
    {
        string temp = Instruction[ifid_ir];
        DECODE_INS=temp;
      

        if (ifid_ir >= Instruction.size())
        {
            return;
        }

        //  cout<<temp<<endl;
        string opcode = temp.substr(25, 7);
      
        if (opcode == "0110011")
        { idex_imm_upper = -1;
            if (temp[1] == '1')
            {
                idex_control = 3;
            }
            else if (temp[6] == '1')
            {
                idex_control = 2;
            }
            else
            {
                idex_control = 1;
            }
            idex_reg1 = stoi(temp.substr(12, 5), 0, 2);
            idex_reg2 = stoi(temp.substr(7, 5), 0, 2);
            idex_imm_lower = -1;
            idex_function3 = stoi(temp.substr(17, 3), 0, 2);
            idex_regd = stoi(temp.substr(20, 5), 0, 2);
        }
        else if (opcode == "0010011" || opcode == "0000011")
        {
            idex_imm_upper = -1;
            idex_reg1 = stoi(temp.substr(12, 5), 0, 2);
            idex_reg2 = -1;
            idex_function3 = stoi(temp.substr(17, 3), 0, 2);
            idex_regd = stoi(temp.substr(20, 5), 0, 2);

            if (temp[1] == '1' && temp[0] == '0' && opcode == "0010011")
            {
                idex_control = 2 + 4;
            }
            else if (opcode == "0000011")
            {
                idex_control = (idex_function3) * 32 + 8 + 4;
                //  cout << idex_control << number_to_binary(idex_control, 32) << "     " << idex_function3 << endl;
            }
            else
            {
                idex_control = 1 + 4;
            }
            if (temp[0] == '1')
            {
                string nul = "11111111111111111111" + temp.substr(0, 12);
                unsigned long long unsignedInt = std::stoul(nul, nullptr, 2);
                idex_imm_lower = static_cast<int>(unsignedInt);
            }
            else
            {
                idex_imm_lower = stoi(temp.substr(0, 12), 0, 2);
            }
        }
        else if (opcode == "0100011")
        {

            idex_imm_upper = -1;
            idex_reg1 = stoi(temp.substr(12, 5), 0, 2);
            idex_reg2 = stoi(temp.substr(7, 5), 0, 2);
            idex_imm_lower = stoi(temp.substr(20, 5) + temp.substr(0, 7), 0, 2);
            idex_function3 = stoi(temp.substr(17, 3), 0, 2);
            idex_regd = -1;
            idex_control = (idex_function3) * 32 + 16 + 4;
        }
        else if (opcode == "1100011")
        {
            if (temp[0] == '1')
            {
                string nul = "111111111111111111111" + temp.substr(0, 1) + temp.substr(24, 1) + temp.substr(1, 6) + temp.substr(20, 3);
                unsigned long long unsignedInt = std::stoul(nul, nullptr, 2);
                idex_imm_upper = static_cast<int>(unsignedInt);
            }
            else
            {
                idex_imm_upper = stoi(temp.substr(0, 1) + temp.substr(24, 1) + temp.substr(1, 6) + temp.substr(20, 3), 0, 2);
            }
            idex_reg2 = stoi(temp.substr(7, 5), 0, 2);
            idex_reg1 = stoi(temp.substr(12, 5), 0, 2);
            idex_function3 = stoi(temp.substr(17, 3), 0, 2);
            idex_regd = -1;
            idex_imm_lower = -1;
            idex_control = 256 + idex_function3 * 512;
        }
        else if(opcode=="0110111")
        {

           
            idex_imm_upper = -1;
            idex_reg1 = -1;
            idex_reg2 =-1;
            idex_imm_lower = stoi(temp.substr(0, 20) , 0, 2);
           
            idex_function3 = 1;
            idex_regd = stoi(temp.substr(20, 5),0,2);
            idex_control = 4096; 
           
        }
        s1 = false;
        s2 = false;
        stall+=1;
        if (idex_reg1 >= 0)
        {
            if (GPR_VALID[idex_reg1] == 0)
            {
                idex_reg1 = GPR[idex_reg1];
            }
            else
            {
                idex_control = -1;
              
                s1 = true;

                return;
            }
        }

        if (idex_reg2 >= 0)
        {
            if (GPR_VALID[idex_reg2] == 0)
            {
                idex_reg2 = GPR[idex_reg2];
            }
            else
            {
                idex_control = -1;
                s2 = true;
                
                return;
            }
        }
        stall-=1;
           cout <<"cycle"<<cycle<<"     DECODE      " <<Instruction_Decoder(DECODE_INS)<<endl;
      
        if (ifid_npc > 0)
        {
            idex_pc = ifid_npc;
            pc = idex_pc;
        }

        if (idex_regd > 0)
        {
            GPR_VALID[idex_regd] += 1;
        }
    }

    if (rev)
    {
        fetch(cycle);
          
    }
    else
    {
        execute(cycle);
    }
}
void execute(int cycle)
{
    if (idex_control > 0)
    {EXECUTE_INS= DECODE_INS;
cout <<"cycle"<<cycle<<"       Execute      " <<Instruction_Decoder(EXECUTE_INS)<<endl;
        exmo_control = idex_control;
                                    
        exmo_regd = idex_regd;
        exmo_reg2 = idex_reg2;
        int temp = idex_reg2;
        if ((idex_control & 4) == 4)
        {
            temp = idex_imm_lower;
        }

        if ((idex_control & 3) == 3)
        {
            if ((idex_function3 & 3) == 0)
            {
                exmo_aluout = idex_reg1 - temp;
            }
            else
            {
                int sign = (idex_reg1 > 0) ? 0 : 1;
                if (sign)
                {
                    exmo_aluout = idex_reg1 >> temp;
                    if (exmo_aluout > 0)
                    {
                        exmo_aluout = -1 * exmo_aluout;
                    }
                }
                else
                {
                    exmo_aluout = idex_reg1 >> temp;
                }
            }
        }
        else if ((idex_control & 3) == 2)
        {
            if (idex_function3 == 0)
            {
                exmo_aluout = (idex_reg1 * temp) & (4294967295);
            }
            else if (idex_function3 == 1)
            {
                exmo_aluout = idex_reg1 * temp;
            }

            else if (idex_function3 == 2)
            {
                exmo_aluout = (idex_reg1 * temp);
            }
            else if (idex_function3 == 3)
            {
                exmo_aluout = (idex_reg1 * temp);
            }

            else if (idex_function3 == 4)
            {
                exmo_aluout = (int)(idex_reg1 / temp);
            }
            else if (idex_function3 == 5)
            {
                exmo_aluout = (int)(idex_reg1 / temp);
            }
            else if (idex_function3 == 6)
            {
                exmo_aluout = idex_reg1 % temp;
            }
            else if (idex_function3 == 7)
            {
                exmo_aluout = idex_reg1 % temp;
            }
        }
        else if ((idex_control & 3) == 1)
        {
            if (idex_function3 == 0)
            {
                exmo_aluout = idex_reg1 + temp;
            }
            else if (idex_function3 == 1)
            {
                exmo_aluout = idex_reg1 << temp;
            }

            else if (idex_function3 == 2)
            {
                exmo_aluout = (idex_reg1 < temp) ? 1 : 0;
            }
            else if (idex_function3 == 3)
            {
                exmo_aluout = (idex_reg1 > temp) ? 1 : 0;
            }
            else if (idex_function3 == 4)
            {
                exmo_aluout = idex_reg1 ^ temp;
            }
            else if (idex_function3 == 5)
            {
                exmo_aluout = idex_reg1 >> temp;
            }
            else if (idex_function3 == 6)
            {
                exmo_aluout = idex_reg1 | temp;
            }
            else if (idex_function3 == 7)
            {
                exmo_aluout = idex_reg1 & temp;
            }
        }
   if(idex_control==4096)
    {  
        exmo_aluout=idex_imm_lower*4096;
       
    }
        if ((idex_control & 8) == 8 || (idex_control & 16) == 16)
        {

            exmo_aluout = idex_reg1 + temp;
        }
        if ((idex_control & 256) == 256)
        {
            int fun3 = idex_control >> 9;

            bool Branch = false;
            // cout<<idex_reg1<<"     "<<temp<<"     "<<fun3<<"     "<<endl;
            if (fun3 == 0)
            {
                if (idex_reg1 == temp)
                {
                    Branch = true;
                }
            }
            else if (fun3 == 1)
            {
                if (idex_reg1 != temp)
                {
                    Branch = true;
                }
            }
            else if (fun3 == 4)
            {
                if (idex_reg1 > temp)
                {
                    Branch = true;
                }
            }
            else if (fun3 == 5)
            {
                if (idex_reg1 <= temp)
                {
                    Branch = true;
                    // cout<<"yesssss"<<endl;
                }
            }
            else if (fun3 == 6)
            {
                int rs1 = (unsigned int)idex_reg1;
                int ttemp = (unsigned int)temp;
                if (rs1 < temp)
                {
                    Branch = true;
                }
            }
            else if (fun3 == 7)
            {
                int rs1 = (unsigned int)idex_reg1;
                int ttemp = (unsigned int)temp;
                if (rs1 >= temp)
                {
                    Branch = true;
                }
            }
            if (Branch)
            { // cout<<"old pc"<<Instruction[pc]<<endl;
                if (idex_pc == Instruction.size())
                {
                    FLAG2 = false;
                }

                pc = idex_pc - 1 + idex_imm_upper;
                // cout<<"new pc"<<Instruction[pc]<<endl;
                ifid_ir = -1;
                ifid_npc = -1;
                ifid_dpc = -1;
                idex_pc = -1;
                idex_imm_upper = -1;
                idex_control = -1;
                idex_reg1 = -1;
                idex_reg2 = -1;
                idex_imm_lower = -1;
                idex_function3 = -1;
                idex_regd = -1;
                flash += 2;
                while (!(FLAG2))
                {
                    write_Back(cycle);
                }
                FLAG2 = true;
            }
            else
            {
                FLAG2 = true;
            }
        }
    }
  

    exmo_control = idex_control;
    if (rev)
    {
        decode(cycle);
    }
    else
    {
        memory(cycle);
    }
}
void memory(int cycle)
{

    if (exmo_control >= 0)
    {  MEMORY_INS=EXECUTE_INS;
     cout<<"cycle"<<cycle<<"    Memory      " <<Instruction_Decoder(MEMORY_INS)<<endl;

        if ((exmo_control & 8) == 8)
        {  if(exmo_aluout<0 || exmo_aluout>1023)
        {
           cout<<" memory access exceeds bounds"<<endl;
        }
            mowb_regd = exmo_regd;
            int Mem_fun = (exmo_control & 7 * 32) >> 5;
            int mem_var = MEMORY[exmo_aluout];
            int sign = (mem_var >= 0) ? 1 : -1;
            if (Mem_fun == 0)
            {
                mowb_aluout = sign * ((mem_var) & (255));
            }
            else if (Mem_fun == 1)
            {
                mowb_aluout = sign * ((mem_var) & (65535));
            }
            else if (Mem_fun == 2)
            {
                mowb_aluout = MEMORY[exmo_aluout];
            }
            else if (Mem_fun == 4)
            {
                mowb_aluout = (MEMORY[exmo_aluout]) & (255);
            }
            else if (Mem_fun == 5)
            {
                mowb_aluout = (MEMORY[exmo_aluout]) & (65535);
            }

            mowb_regd = exmo_regd;
            cout << "EA: " << exmo_aluout << "  MEMORY: " << mowb_aluout << endl;
        }
        else if ((exmo_control & 16) == 16)
        {   if(exmo_aluout<0 || exmo_aluout>1023)
        {
           cout<<" memory access exceeds bounds"<<endl;
        }
            int str_fun = (exmo_control & 7 * 32) >> 5;
            if (str_fun == 0)
            {
                MEMORY[exmo_aluout] = (exmo_reg2) & (255);
            }
            else if (str_fun == 1)
            {
                MEMORY[exmo_aluout] = (exmo_reg2) & (65535);
            }
            else if (str_fun == 2)
            {
                MEMORY[exmo_aluout] = (exmo_reg2);
            }
            cout << "EA: " << exmo_aluout << "  MEMORY: " << MEMORY[exmo_aluout] << endl;
        }
        else if(exmo_control==4096){
             mowb_aluout = exmo_aluout;
            mowb_ldout = exmo_aluout;
            mowb_regd = exmo_regd;

        }
        else
        {

            mowb_aluout = -1;
            mowb_ldout = exmo_aluout;
            mowb_regd = exmo_regd;
        }
    }

    mowb_control = exmo_control;

    if (rev)
    {
        execute(cycle);
    }
    else
    {
        write_Back(cycle);
    }
}
void write_Back(int cycle)
{
    if (mowb_control >= 0)
    {   WRITE_BACK_INS=MEMORY_INS;
        cout<<"cycle"<<cycle<<"     WRITE-BACK      " <<Instruction_Decoder(WRITE_BACK_INS)<<endl;
        if ((mowb_control & 8) == 8 && mowb_regd > 0)
        {
            GPR[mowb_regd] = mowb_aluout;
            GPR_VALID[mowb_regd] -= 1;
        }
        else if(mowb_control==4096)
        {
            GPR[mowb_regd] = mowb_aluout;
            GPR_VALID[mowb_regd] -= 1; 
        }
        else if (mowb_control < 8)
        {
            if (mowb_regd > 0)
            {

                GPR[mowb_regd] = mowb_ldout;
                GPR_VALID[mowb_regd] -= 1;
            }
        }

        // cout<<"we are writing to rd:"<<mowb_regd<<endl;
    }

    if (rev)
    {
        memory(cycle);
    }
}
void cpu(vector<string> Instruction)
{

    for (int i = 1; i <= 1000001; i++)
    {
        if (rev)
        {
            write_Back(i);
            // print_state();
            // cout<<"FLAG"<<FLAG<<endl;
        }
        else
        {
            fetch(i);
        }
        if (FLAG)
        {
            if (rev)
            {
                write_Back(i+1);
                write_Back(i+2);
                write_Back(i+3);
              //  write_Back();
              //  write_Back();

                printf("end of instructions\n");
                cout << "total no of cycles:" << i + 3 << endl;
                //  print_state();
            }
            else
            {
                printf("end of instructions\n");
                cout << "total no of cycles:" << i << endl;
                //  print_state();
            }
            break;
        }
        if (i == 100000)
        {
            printf("!!!!InFINITE_LOOP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! or no.of cycles limit exceeded\n");
        }
    }
    GPR[0] = 0;
}
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    char* decoder (char* input) {
         create_map();
        
        string input2=input;
        if(input2[input2.length()-1]!='\n')
        {
            input2+='\n';
        }
        
        parse_text(input2);
                 

   std::string output;
    for (int i=0;i<Instruction.size();i++) {
        output += Instruction[i]+ "\n";
      // Use a suitable delimiter
    }
        
        char* c_output = strdup(output.c_str());
    return c_output;
    }
    char* ins_decoder (char* input) {
        
      vector<string> ins_temp;
      string input2=input;
      int l=input2.length();
      int i=0;
      int count=0;
       std::string output;
       string temp="";
      while(i<l)
      { if (input2[i]!='\n')
      {
        temp+=input2[i];
        
      }
      else
      { 
        ins_temp.push_back(temp);
        temp="";
      }
        i+=1;

      }
                 
  
    for (int i=0;i<ins_temp.size();i++) {
       
        output += Instruction_Decoder(ins_temp[i])+'\n';
      // Use a suitable delimiter
    }
        
        char* c_output = strdup(output.c_str());
    return c_output;
    }
 char* pipeline (char* input) {
         create_map();
        
        string input2=input;
        if(input2[input2.length()-1]!='\n')
        {
            input2+='\n';
        }
        
        parse_text(input2);
        create_stages();
        cpu(Instruction);
        print_state();
                 

 string output="successful-run";
   
        char* c_output = strdup(output.c_str());
    return c_output;
    }

   
   
}
int main()
{
    create_map();
    rev = true;
    //  parse_text("addi x19,x19,9\naddi x20,x19,9\nadd x23,x19,x20\naddi x24,x23,7\n");
    // parse_text("addi x3,x3,91\naddi x4,x4,71\naddi x2,x2,1021\nadd x5,x2,x3\nsub x6,x5,x5\nslli x7,x7,4\naddi x3,x5,123\nadd x6,x3,x7\n");
    // rev=true;
    // parse_text("addi x5,x5,11\nbeq x5,x0,.Loop2\n.Loop:\nadd x16,x16,x5\naddi x5,x5,-1\nbne x5,x3,.Loop\n.Loop2:\naddi x10,x10,957\naddi x16,x16,1\n" );

    // parse_text("addi x11,x11,9\naddi x11,x11,24\n");

   
//parse_text("addi x10,x10,4\naddi x11,x11,8\nsb x11,0(x10)\nlb x12,0(x10)\naddi x13,x13,1\naddi x15,x15,1\nbge x12,x13,.Loop\n.Loop:\naddi x13,x13,1\naddi x14,x14,5\naddi x10,x10,4\naddi x15,x15,1\nsb x15,0(x10)\nbge x12,x13,.Loop\naddi x20,x20,1000\naddi x16,x0,1\naddi x18,x0,0\naddi x19,x0,0\naddi x15,x15,-1\nbge x15,x16,.Loop2\n.Loop2:\nlb x17,0(x10)\naddi x10,x10,-4\naddi x15,x15,-1\nand x20,x17,x16\nbeq x20,x16,.Loop3\naddi x20,x0,0\naddi x18,x18,1\nbge x15,x16,.Loop2\n.Loop3:\naddi x19,x19,1\nbge x15,x16,.Loop2\nsb x18,10(x10)\nsb x19,11(x10)\n");
//parse_text("bne x10,x11,LABEl2\nLABEl2:\nadd x10,x11,x12\nLabel22:\naddi x10,x10,7\nbeq x10,x11,Label22\n");
 //parse_text("      BNE                X10  ,           x11                ,LABEl2\nLABEl2:\nadd x10,x11,x12\nLabel22:\naddi x10,x10,7\nbeq x10,x11,Label22\n");
 //parse_text2("BEQ X11,X12,9\n");
 //parse_text("lui x3,100\naddi x3,x3,234\n");
//     for (int i = 0; i < Instruction.size(); i++)
//     {
//         cout << Instruction[i] << endl;
//         cout <<Instruction_Decoder(Instruction[i])<<endl;
//     }
//  create_stages();
//       cpu(Instruction);
//       print_state();

    // cout<<"bne x15,x0,.Loop\n.Loop:\nadd x17,x17,x15\naddi x2,x2,3\nxori x2,x2,3\nsub x15,x15,x16\nbne x15,x0,.Loop\nsw x17,0(x11)\n"<<endl;
    return 0;
}
