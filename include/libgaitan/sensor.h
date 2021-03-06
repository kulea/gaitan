/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SENSOR_H
#define SENSOR_H
#include <string>

#include <libgaitan/table.h>
using namespace std;


namespace gaitan
{
class Sensor
{
  public:
    Table *data;
    
  public:	
    Sensor();
    Sensor(int col, int raw);
    Sensor(string filename); 
    virtual ~Sensor();
    
    virtual int load(const string & filename);
    int save(const string &filename, const int &precision=15);
    void print(const int  & beg, const int & end);
    void print();
    void initData();
    
    
    virtual int acquire(const std::string &path , bool flagDisp=true)=0;
   
    static Table* synchronizeZeroBloc(Sensor* s1, Sensor* s2);
 
};
}
#endif // SENSOR_H
