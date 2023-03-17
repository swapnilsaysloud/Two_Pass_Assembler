/* 
NAME: Swapnil Saraswat
ROLL NO: 2101CS76
COURSE: CS209-CS210 Architechture LAB
INSTRUCTOR NAME: Dr. Jimson Matthew
declaration of authorship: I hereby declare this assemler is made by me as part of the mini project for this course

*/


#include <bits/stdc++.h>
using namespace std;
class literal
{
public:
  int literal;
  int address;
};
 unordered_map<string,pair<string,int>> Mnemonics;


 void Mnemonics_Declare()
 {
 Mnemonics["ldc"] = {"00000000",1};
 Mnemonics["adc"] = {"00000001", 1};
 Mnemonics["ldl"]={"00000010", 4};
 Mnemonics["stl"]={"00000011", 4};
 Mnemonics["ldnl"]={"00000100", 4};
 Mnemonics["stnl"]={"00000101", 4};
 Mnemonics["add"]={"00000110", 0};
 Mnemonics["sub"]={"00000111", 0};
 Mnemonics["shl"]={"00001000", 0};
 Mnemonics["shr"]={"00001001", 0};
 Mnemonics["adj"]={"00001010", 1};
 Mnemonics["a2sp"]={"00001011", 0};
 Mnemonics["sp2a"]={"00001100", 0};
 Mnemonics["call"]={"00001101", 4};
 Mnemonics["return"]={"00001110", 0};
 Mnemonics["brz"]={"00001111", 4};
 Mnemonics["brlz"]={"00010000", 4};
 Mnemonics["br"]={"00010001", 4};
 Mnemonics["HALT"]={"00010010", 0};
 Mnemonics["data"]={"NULL", 2};
 Mnemonics["SET"]={"NULL", 3};
 }

vector<pair<string,int>> Sym_Table; // TO STORE LABELS WITH THEIR ADDRESS
vector<literal> LiteralTable;       // TO STORE LITERALS WITH THEIR ADDRESS
vector<pair<int, string>> errors;   // TO STORE ERRORS PRESENT IN THE INPUT FILE
map<string, int> labels;            //MAPS LABELS TO AVOID DUPLICACY
vector<string> CleanedLines;        // STORES LINES WITHOUT COMMENTS AND SPACES FOR PASS 2 TO AVOID OPENING FILES MULTIPLE TIMES
vector<pair<int, string>> ListingLines; // STORES LISTING FILE INSTRUCTIONS
vector<string> ObjectCode;              //STORES OBJECT CODE
string Uploaded_File;                   // SAVES NAME OF THE UPLOADED FILE
//  Removes comment from a line
string remove_Comment(string line)
{
  string a;
  int num = line.find(';');
  if (num > 0)
    a = line.substr(0, num - 1);
  if (num == 0)
    a = "";
  if (num == -1)
    return line;
  return a;
}
// Removes ':'. RETURN 1 if ':' is present else 0
bool hasLabel(string line)
{
  int num = -1;
  num = line.find(':');
  if (num >= 0)
  {
    return 1;
  }
  return 0;
}
void generateErrors(int line, string type)
{
  // This functions stores the errors in errors vector
  errors.push_back({line + 1, "Error at line: " + to_string(line) + " -- Type: " + type});
  cout << "ERROR at line: " << line << " type: " << type << endl;
}
string CleanLine(string &line, int lineNo)
{
  line = remove_Comment(line); // removes all comments
  for (int i = 0; i < 2; i++)  // removes empty spaces from starting and end
  {
    reverse(line.begin(), line.end());
    while (line.back() == ' ' or line.back() == '\t')
    {
      line.pop_back(); // Popping empty spaces
    }
  }
  return line;
}
// To check if label is valid
bool isValidLabel(string label)
{
  int l = label.size();
  if (l == 0 || (!isalpha(label[0]) && label[0] != '_'))
    return false;
  for (int i = 0; i < l; i++)
  {
    if (!isalnum(label[i]) && label[i] != '_')
      return false;
  }
  return true;
}
// Return 1 if valid number else 0
bool isValidNumber(string word)
{
  bool result = true;
  if (word[0] == '+' || word[0] == '-') // To allow cases when +x or -x in inputted with mnemonics like ldc
  {
    reverse(word.begin(), word.end());
    word.pop_back();
    reverse(word.begin(), word.end());
  }
  for (char ch : word) // Check other digits after removing + or - sign
  {
    if (!isdigit(ch))
    {
      result = false;
    }
  }
  return result;
}
// returns number of operand required if that mnemonic is present in mnemonic table, else returns 0
int is_ValidMnemonic(string mnemonic)
{
  for (auto x: Mnemonics)
  {
    if (x.first.compare(mnemonic) == 0)
      return x.second.second + 1; // added 1 because some mnemonics require 0 operands as well, so to differentiate with return 0, we add 1.
  }
  return 0;
}
// maintains record of labels that are inserted in symbol tabel to avoid duplicacy
bool MapLabels(int lineNo, string label, bool isDuplicate)
{
  labels[label]++;
  if (labels[label] >= 2)
  {
    generateErrors(lineNo, "Multiple declaration of same label");
    isDuplicate = true;
  }
  return isDuplicate;
}
void seeErrors()
{
  // Stores the errors or warnings in the file
  string Error_file = Uploaded_File.substr(0, Uploaded_File.find(".", 0)) + ".log";
  ofstream outErrors(Error_file);
  if (errors.empty())
  {
    outErrors << "No errors in Pass 1" << endl;
    outErrors.close();
  }
  sort(errors.begin(), errors.end());
  for (auto to : errors)
  {
    outErrors << to.second << endl; // Prints only the error messages in file
  }
  outErrors.close();
}
int firstPass(string file)
{
  int PC = 0, err = 0, line_num = 0;
  string line, word, label;
  ifstream assembly(file);
  while (getline(assembly, line))
  {
    vector<string> a;
    line_num++;
    line = CleanLine(line, line_num); // removes comments and unwanted spaces
    CleanedLines.push_back(line);     // Storing cleaned lines for second pass
    stringstream s(line);
    string label, mnemonic, operand;

    if (line.empty())
      continue;
    if (line.find(':') != string::npos) // if : is present
    {
      s >> label;
      label.pop_back();                                     // To remove colon
      bool isDuplicate = MapLabels(line_num, label, false); // Will check for duplicate labels
      if (isDuplicate == true)
        continue;
      if (isValidLabel(label))
      {
        bool push = true;
        for (int i = 0; i < Sym_Table.size(); i++)
        {
          if (Sym_Table[i].first == label && Sym_Table[i].second == -1) // If this label was declared earlier as an operand, so we put its PC value here
          {
            Sym_Table[i].second = PC;
            push = false; // So that we don't push it again
          }
        }
        if (push == true)
          Sym_Table.push_back({label, PC}); // For labels declared for first time
      }
      else
      {
        generateErrors(line_num, "Invalid label");
      }
    }
    s >> mnemonic;
    if (mnemonic == "") // continue if there is nothing after label, without incrementing PC
      continue;
    int NOofOperand = is_ValidMnemonic(mnemonic);
    cout << "no of operand = " << NOofOperand << endl;
    cout << "Mnemonic=" << mnemonic << endl;
    cout << "Line=" << line << endl;
    if (NOofOperand == 0)
    {
      generateErrors(line_num, "Invalid Mnemonic");
      continue;
    }
    s >> operand; // remove , space /

    bool OperandIsNumber = isValidNumber(operand); // To check if operand is a label or number
    bool OperandIsLabel = isValidLabel(operand);
    if (mnemonic == "SET" || mnemonic == "set") // Implementing SET instructions
    {
      for (int i = 0; i < Sym_Table.size(); i++)
      {
        if (Sym_Table[i].first == label)
        {
          if (isValidNumber(operand))
            Sym_Table[i].second = stoi(operand);
          else
          {
            generateErrors(line_num, "Operand for SET instruction must be a number");
          }
        }
      }
    }
    if (NOofOperand > 1)
    {
      // operand--label push into symbol table
      if (operand == "") // Operand was required but is missing
      {
        generateErrors(line_num, "Operand missing");
        continue;
      }
      if (OperandIsLabel == false && OperandIsNumber == false)
      {
        cout << operand << endl;
        generateErrors(line_num, "Invalid Operand");
        continue;
      }
      // Push into symbol table if operand is a label
      if (OperandIsLabel == true && OperandIsNumber == false && labels[operand] == 0)
      {
        bool push = true;
        for (int i = 0; i < Sym_Table.size(); i++)
        {
          if (Sym_Table[i].first == operand)
            push = false;
        }
        if (push == true)
          Sym_Table.push_back({operand, -1});

        // SymbolTable.push_back({operand,PC});
      }
      else if (OperandIsLabel == false && OperandIsNumber == true)
      {
        if (NOofOperand < 3) // if not data or set
        {
          LiteralTable.push_back({stoi(operand), -1});
        }
      }
    }
    // check if there is something or not
    string extra;
    s >> extra;
    if (!extra.empty())
      generateErrors(line_num, "Extra on end of line");
    PC++;
  }

  // LTORG loop for literals
  PC--;
  for (int i = 0; i < LiteralTable.size(); i++)
  {
    if (LiteralTable[i].address == -1)
    {
      LiteralTable[i].address = PC++;
    }
  }
  
  
  cout << "PRINTING SYMBOL TABLE" << endl;
  for (auto x : Sym_Table)
  {
    cout << x.first << "  " << x.second << endl;
  }

  /* TO PRINT LITERAL TABLE
  cout << "PRINTING LITERAL TABLE" << endl;
  for (int i = 0; i < LiteralTable.size(); i++)
    cout << LiteralTable[i].literal << " " << LiteralTable[i].address << endl; */
  seeErrors();

  return 0;
}
string convertTo24bit(int x) // converts number to 24 bit
{
  string c = "000000000000000000000000";
  for (int i = 23; i >= 0; i--)
  {
    int k = x >> i;
    if (k & 1)
      c[23 - i] = '1';
    else
      c[23 - i] = '0';
  }
  return c;
}

string convertTo32bit(int x) //converts number to 32 bit
{
  string c = "00000000000000000000000000000000";
  for (int i = 31; i >= 0; i--)
  {
    int k = x >> i;
    if (k & 1)
      c[31 - i] = '1';
    else
      c[31 - i] = '0';
  }
  return c;
}
string ConvertTo8bit(int x) // converts number to 8 bit
{
  string c = "00000000";
  for (int i = 7; i >= 0; i--)
  {
    int k = x >> i;
    if (k & 1)
      c[7 - i] = '1';
    else
      c[7 - i] = '0';
  }
  return c;
}
string GetOpCode(string mnemonic) // To get opcode of corresponding mnemonic
{
  string a = "";
  for (auto x: Mnemonics)
  {
    if (mnemonic.compare(x.first) == 0)
    {
      a = x.second.first;
    }
  }
  return a;
}
int GetAddress(string label) // Gets address of that label from Symbol Table
{
  for (auto x : Sym_Table)
  {
    if (x.first.compare(label) == 0)
    {
      return x.second;
    }
  }
  return 0;
}
// RETURN 1 if Offset Required else 0
bool requiresOffset(string mnemonic)
{
  string offsetMnemonics[] = {"ldl", "stl", "ldnl", "stnl", "call", "brz", "brlz", "br"};
  for (int i = 0; i < 8; i++)
  {
    if (offsetMnemonics[i].compare(mnemonic) == 0)
      return 1;
  }
  return 0;
}
// gives us the 24 bit code consisting of operand value or address of operand label
string SetOpCode(string operand, string mnemonic, int NoOfOperand, int PC) 
{
  string ans;
  int num;
  // for data mnemonic
  if (NoOfOperand == 3)
  {
    ans = convertTo32bit(stoi(operand));
    return ans;
  }
  if (isValidNumber(operand))
    num = stoi(operand);
  else if (isValidLabel(operand))
    num = GetAddress(operand);
  else
    return "";
  if (requiresOffset(mnemonic) && isValidLabel(operand))
    num -= PC + 1;
  ans = convertTo24bit(num);
  return ans;
}
void createMap(unordered_map<string, char> *um) // Helping Function
{
  (*um)["0000"] = '0';
  (*um)["0001"] = '1';
  (*um)["0010"] = '2';
  (*um)["0011"] = '3';
  (*um)["0100"] = '4';
  (*um)["0101"] = '5';
  (*um)["0110"] = '6';
  (*um)["0111"] = '7';
  (*um)["1000"] = '8';
  (*um)["1001"] = '9';
  (*um)["1010"] = 'A';
  (*um)["1011"] = 'B';
  (*um)["1100"] = 'C';
  (*um)["1101"] = 'D';
  (*um)["1110"] = 'E';
  (*um)["1111"] = 'F';
}

// function to find hexadecimal equivalent of binary  
string convertBinToHex(string bin)
{
  int l = bin.size();
  int t = bin.find_first_of('.');

  // length of string before '.'
  int len_left = t != -1 ? t : l;

  // add min 0's in the beginning to make left substring length divisible by 4
  for (int i = 1; i <= (4 - len_left % 4) % 4; i++)
    bin = '0' + bin;

  // if decimal point exists
  if (t != -1)
  {
    // length of string after '.'
    int len_right = l - len_left - 1;

    // add min 0's in the end to make right substring length divisible by 4
    for (int i = 1; i <= (4 - len_right % 4) % 4; i++)
      bin = bin + '0';
  }
  unordered_map<string, char> bin_hex_map;
  createMap(&bin_hex_map);

  int i = 0;
  string hex = "";

  while (1)
  {
    // one by one extract from left, substring of size 4 and add its hex code
    hex += bin_hex_map[bin.substr(i, 4)];
    i += 4;
    if (i == bin.size())
      break;

    // if '.' is encountered add it to result
    if (bin.at(i) == '.')
    {
      hex += '.';
      i++;
    }
  }
  return hex;
}
void PrintFiles() // Printing into listing and Object files
{
  string Listing_file = Uploaded_File.substr(0, Uploaded_File.find(".", 0)) + ".L";
  ofstream Listing(Listing_file);

  if (ListingLines.empty())
  {
    cout << "No Listing Lines!" << endl;
    Listing.close();
  }
  sort(ListingLines.begin(), ListingLines.end());
  cout << (int)errors.size() << " errors encountered! See logFile.log" << endl;
  for (auto to : ListingLines)
  {
    Listing << to.second << endl;
  }
  Listing.close();
  string Object_file = Uploaded_File.substr(0, Uploaded_File.find(".", 0)) + ".o";
  ofstream Object(Object_file);
  if (ObjectCode.empty())
  {
    cout << "No object code !" << endl;
    Object.close();
  }
  for (auto to : ObjectCode)
  {
    Object << to << endl;
  }
}
string toHex(int num1) // Converts number to hexadecimal format
{
  if (num1 == 0)
    return "0";
  uint8_t num = num1;
  string s = "";
  while (num)
  {
    int temp = num % 16;
    if (temp <= 9)
      s += (48 + temp);
    else
      s += (87 + temp);
    num = num / 16;
  }
  reverse(s.begin(), s.end());
  return s;
}

int SecondPass()
{
  int line_num = 0, PC = 0;

  for (auto instr : CleanedLines) // getting instructions without comments and extra spaces
  {
    line_num++;
    stringstream s(instr);
    string label, mnemonic, operand, machineCode = "", LongOperand;
    if (instr == "")
      continue;
    if (hasLabel(instr)) // We push the labels in listing file with PC
    {
      s >> label;
      label.pop_back();
      ListingLines.push_back({line_num, "000000"+toHex(PC) + "       " + label});
    }
    s >> mnemonic;
    if (mnemonic.empty()) // If there is only label, we don't increment PC and skip the loop
      continue;
    int NoOfOperand = is_ValidMnemonic(mnemonic);
    string Opcode = GetOpCode(mnemonic); // Getting opcode for corresponding mnemonic
    if (NoOfOperand > 1)
    {
      s >> operand;
      LongOperand = SetOpCode(operand, mnemonic, NoOfOperand, PC); // LongOperand gives 
    }
    else
    {
      LongOperand = SetOpCode("0", mnemonic, NoOfOperand, PC);
    }
    machineCode = machineCode + LongOperand; // Adding operand 
    if (NoOfOperand != 3)
      machineCode = machineCode + Opcode;    // appending opcode
    ObjectCode.push_back(machineCode);
    // pushing into listing file
    if(isValidNumber(operand) && NoOfOperand>1) ListingLines.push_back({line_num, "000000"+toHex(PC) + "     " + convertBinToHex(machineCode) + "      " + mnemonic+"       "+"0x"+toHex(stoi(operand))});
    else{
      ListingLines.push_back({line_num, "000000"+toHex(PC) + "     " + convertBinToHex(machineCode) + "      " + instr});
    }
    PC++;
  }
  PrintFiles();
  return 0;
}

int main(int argc, char *argv[])
{
  FILE *assembly;
  Uploaded_File = argv[1];
  cout << "hello" << endl;
  Mnemonics_Declare();
  firstPass(Uploaded_File);
  if (errors.empty())
    SecondPass();
  return 0;
}