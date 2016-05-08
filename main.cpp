//-----------------------------------------------------------------------------
// main.cpp
// Copyright(c) 2015 Jeff Hutchinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#include <string>
#include <string.h>
#include <vector>

inline bool isWordcharacter(char x) 
{
   return (!isspace(x) && iswalnum(x)) || x == '_';
}

inline bool isFunctionCallAtPos(const std::string &fn, const std::vector<std::string> &tokens, size_t currentId) 
{
   return (tokens[currentId].find(fn) != std::string::npos) && ((currentId + 1) < tokens.size()) && (tokens[currentId + 1] == "(");
}

inline bool isVarCompatable(const char c)
{
   return c == '_' || c == ':' || (c >= 0x41 && c <= 0x5A) || (c >= 0x30 && c <= 0x39) || (c >= 0x61 && c <= 0x7A);
}

inline bool isOperator(const char c)
{
   return (c == '+' || c == '-' || c == '/' || c == '*');
}

std::vector<std::string> tokenize(const std::string &str)
{
   std::vector<std::string> tokens;
   size_t len = str.length();
   const char *cs = str.c_str();
   size_t i = 0;
   while (i < len) 
   {
      if (str[i] == '%')
      {
         size_t pos = i + 1;
         if (str[pos])
         {
            // It is %=
            if (str[pos] == '=')
            {
               tokens.push_back("%=");
               i += 2;
               continue;
            }

            // It is a variable!
            std::string var = "%";

            // first character after % cannot be a number. Any characters afterwards can.
            if ((str[pos] >= 0x30 && str[pos] <= 0x39) == false)
            {
               while (str[pos] && isVarCompatable(str[pos]))
               {
                  var += str[pos];
                  ++pos;
                  ++i;
               }
               tokens.push_back(var);
               ++i;
               continue;
            }
         }

         // It's not, it's a modulo operator!
         tokens.push_back("%");
      }
      else if (isOperator(str[i]))
      {
         size_t nextChar = i + 1;
         if (str[nextChar] && 
             ((str[nextChar] == '=') ||
              (str[i] == '+' && str[nextChar] == '+') ||
              (str[i] == '-' && str[nextChar] == '-'))
            )
         {
            // It is op= or ++ or --
            std::string temp;
            temp.append(1, str[i]);
            temp.append(1, str[nextChar]);
            tokens.push_back(temp);
            i += 2;
            continue;
         }

         // regular operator.
         tokens.push_back(str.substr(i, 1));
      }
      else if (ispunct(str[i]) != 0 || !isWordcharacter(str[i]))
      {
         tokens.push_back(str.substr(i, 1));
      }
      else 
      {
         // grab full word and push that as a token.
         size_t l = 0;
         size_t start = i;
         while ((start + l) < len && isWordcharacter(str[start + l]))
            l++;

         std::string tmp = str.substr(start, l);
         tokens.push_back(tmp);

         // Only add if we are greater than 0. If we are zero, we will just
         // grab the increment below.
         if (l > 0) 
         {
            i += l;
            continue;
         }
      }
      i++;
   }
   return tokens;
}

std::string ts2lua(const std::string &src)
{
   std::vector<std::string> &tokens = tokenize(src);

   for (std::string &str : tokens)
      printf("%s\n", str.c_str());

   return "";
}

int main(int argc, const char **argv)
{
#define CODE(str) #str

   ts2lua(CODE(
      function add(%a, %b)
      {
         return %a + %b;
      }

      function helloWorld(%msg)
      {
         %val = 1;
         %mathExpression = %val %1+33*64;
         %val *= 2;
         echo(%msg);

         %ret = add(1, 2);

         new SimObject
         {
            fieldA = 1;
            trigger = new Trigger()
            {

            };
         };
      }
   ));
   system("pause");
   return 0;
}