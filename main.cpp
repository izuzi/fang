#include <iostream>

//
//  main.cpp
//  fang
//
//  Created by izuzi@foxmail.com on 16/10/8.
//

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <string.h>

#include "zload.h"
#include "zjson.h"

using namespace std;
using namespace NS_CZJSON;

class CMyHouseType
{
private:
    map<double, string> mapHouseList;
    vector<double> vecHouseSize;
    
public:
    void Add(double fSize, string strPos)
    {
        string _strPos = "";
        if(Search(fSize, _strPos))
        {
            strPos += string("/") + _strPos;
        }
        else
        {
            vecHouseSize.push_back(fSize);
            sort(vecHouseSize.begin(), vecHouseSize.end());
        }
        
        mapHouseList[fSize] = strPos;
    }
    
    bool Search(double fSize, string& strPos)
    {
        map<double, string>::iterator it;
        
        it = mapHouseList.find(fSize);
        
        if(it == mapHouseList.end())
        {
            return false;
        }
        
        strPos = it->second;
        
        return true;
    }
    
    uint32_t Size()
    {
        return vecHouseSize.size();
    }
    
    double Index(uint32_t u32Id)
    {
        return vecHouseSize[u32Id];
    }
    
    void Show()
    {
        for(int i=0; i<Size(); i++)
        {
            cout<<Index(i)<<endl;
        }
    }
};

class CMyHousePlan
{
private:
    bool bInitFlag;
    CMyHouseType oHouseType;
    
    double fErrSize;
    double fMyArea;
    uint32_t u32MyNum;
    
    vector< vector<double> > vecResult;
    
    
public:
    CMyHousePlan(string strConf) : bInitFlag(false)
    {
        Init(strConf);
    }
    
    ~CMyHousePlan()
    {

    }
    
    void Done(double _fErrSize, double _fMyArea, uint32_t _u32MyNum, uint32_t u32MaxSize, uint32_t u32MinSize)
    {
        if(!bInitFlag) return;
        
        Choose(_fErrSize, _fMyArea, _u32MyNum);
        Order();
        Show(u32MaxSize, u32MinSize);
    }
    
private:
    bool Init(string strConf)
    {
        CZLoad oZload(strConf);
        bInitFlag = false;
        
        vector< vector<string> > vecConfData;
        if(oZload.Read(vecConfData))
        {
            for(int i=0; i<vecConfData.size(); i++)
            {
                double fSize = atof(vecConfData[i][0].c_str());
                string strPos = vecConfData[i][1];
                
                oHouseType.Add(fSize, strPos);
            }
            
            bInitFlag = true;
        }
        
        return bInitFlag;
    }
    
    void Choose(double _fErrSize, double _fMyArea, uint32_t _u32MyNum)
    {
        fErrSize = _fErrSize;
        fMyArea = _fMyArea;
        u32MyNum = _u32MyNum;
        
        vecResult.clear();
        
        for(int i=oHouseType.Size() - 1; i>=0; i--)
        {
            uint32_t u32HouseNum = 0;
            double fSumArea = 0;
            vector<double> vecPlan;
            
            _GetOne(i, u32HouseNum, fSumArea, vecPlan);
        }
    }
    
    int isRight(double fTotleArea, double fSumArea)
    {
        double fLastArea = fTotleArea - fSumArea;

        if(fabs(fLastArea) <= fErrSize )
        {
            return 0;
        }
        
        if(fLastArea < 0)
        {
            return -1;
        }
        
        if(fLastArea > 0)
        {
            return 1;
        }
    }
    
    void GetLastTwo(int i, double fLastArea, vector<double> vecPlan)
    {
        int beg = 0;
        int end = i;
        while(beg <= end)
        {
            int ret = isRight(fLastArea, oHouseType.Index(beg) + oHouseType.Index(end));
            
            if(ret == 0)
            {
                double fDiff = fLastArea - (oHouseType.Index(beg) + oHouseType.Index(end));

                vecPlan.push_back(oHouseType.Index(end));
                vecPlan.push_back(oHouseType.Index(beg));
                vecPlan.push_back(fLastArea - oHouseType.Index(beg) - oHouseType.Index(end));

                vecResult.push_back(vecPlan);

                vecPlan.pop_back();
                vecPlan.pop_back();
                vecPlan.pop_back();

                if(fDiff > 0)
                    beg++;
                else
                    end--;

                continue;
            }
            
            if(ret < 0)
            {
                end--;
                continue;
            }
            
            if(ret > 0)
            {
                beg++;
                continue;
            }
        }
        
        if(0 == isRight(fLastArea, oHouseType.Index(beg) + oHouseType.Index(beg)))
        {
            vecPlan.push_back(oHouseType.Index(beg));
            vecPlan.push_back(oHouseType.Index(beg));
            vecPlan.push_back(fLastArea - oHouseType.Index(beg) - oHouseType.Index(beg));

            vecResult.push_back(vecPlan);

            vecPlan.pop_back();
            vecPlan.pop_back();
            vecPlan.pop_back();
        }

        if(0 == isRight(fLastArea, oHouseType.Index(end) + oHouseType.Index(end)))
        {
            vecPlan.push_back(oHouseType.Index(end));
            vecPlan.push_back(oHouseType.Index(end));
            vecPlan.push_back(fLastArea - oHouseType.Index(end) - oHouseType.Index(end));

            vecResult.push_back(vecPlan);

            vecPlan.pop_back();
            vecPlan.pop_back();
            vecPlan.pop_back();
        }
    }
    

    void _GetOne(int i, uint32_t u32HouseNum, double fSumArea, vector<double> vecPlan)
    {
        u32HouseNum++;
        fSumArea += oHouseType.Index(i);
        vecPlan.push_back(oHouseType.Index(i));

        if(i < 0 || u32HouseNum > u32MyNum)
        {
            return;
        }


        if(u32HouseNum == u32MyNum && fabs(fMyArea - fSumArea) <= fErrSize )
        {
            vecPlan.push_back(fMyArea - fSumArea);
            vecResult.push_back(vecPlan);
            return;
        }
               
        
        for(int j=i; j>=0; j--)
        {
            _GetOne(j, u32HouseNum, fSumArea, vecPlan);
        }
    }


    void GetOne(int i, uint32_t u32HouseNum, double fSumArea, vector<double> vecPlan)
    {
        if(i < 0 || u32HouseNum > u32MyNum)
        {
            return;
        }
        
        u32HouseNum++;
        fSumArea += oHouseType.Index(i);
        vecPlan.push_back(oHouseType.Index(i));
        
        if(u32HouseNum == (u32MyNum - 2))
        {
            GetLastTwo(i, fMyArea - fSumArea, vecPlan);
            return;
        }

        for(int j=i; j>=0; j--)
        {
            GetOne(j, u32HouseNum, fSumArea, vecPlan);
        }
    }
    
    void Order()
    {
        for (int i=0; i<vecResult.size(); i++)
        {
            double fMax = i;
            
            for (int j = i+1; j<vecResult.size(); j++)
            {
                if(vecResult[j].back() > vecResult[fMax].back())
                {
                    fMax = j;
                }
            }
  
            vector<double> vecTmp = vecResult[i];
            vecResult[i] = vecResult[fMax];
            vecResult[fMax] = vecTmp;
        }
    }
    
    void Show(uint32_t u32MaxSize, uint32_t u32MinSize)
    {
        CZJson zjResult;
        
        for (int i = 0; i<vecResult.size(); i++)
        {
            bool bFlag = true;
            for (int j = 0; j<vecResult[i].size()-1; j++)
            {
                if(FilterMax(vecResult[i][j], u32MaxSize) || FilterMin(vecResult[i][j], u32MinSize))
                {
                    bFlag = false;
                    break;
                }
            }
            
            if(bFlag == false)
            {
                continue;
            }
            
            CZJson zjPlan;
            
            for (int j = 0; j<vecResult[i].size(); j++)
            {
                int k = (j + vecResult[i].size() - 1) % vecResult[i].size();
                
                if(k == vecResult[i].size() - 1)
                {
                    zjPlan.Set("err", ToString(vecResult[i][k])).End();
                }
                else
                {
                    string strPos = "";
                    oHouseType.Search(vecResult[i][k], strPos);
                    
                    vector<string> vecPos;
                    StrSplit(strPos.c_str(), "/", vecPos);
                    CZJson zjPos;
                    for(int x=0; x<vecPos.size(); ++x)
                    {
                        zjPos.Set(vecPos[x]);
                    }
                    zjPos.End();
                    
                    zjPlan.Set("area", ToString(vecResult[i][k])).Set("pos", zjPos).End("type", LIST);
                }
            }
            
            zjResult.Set( zjPlan ).End("", LIST);
        }
        
        cout << zjResult.ToString() << endl;
    }
    
    bool FilterMax(const double fSize, const double fMax)
    {
        if(fSize > fMax)
        {
            return true;
        }
        
        if(0 == fMax)
        {
            return false;
        }
        
        return false;
    }
    
    bool FilterMin(const double fSize, const double fMin)
    {
        if(fSize < fMin)
        {
            return true;
        }
        
        if(0 == fMin)
        {
            return false;
        }
        
        return false;
    }
    
    bool FilterNo(const string strPos, const string strNo)
    {
        if(strstr(strPos.c_str(), strNo.c_str()) == NULL)
        {
            return true;
        }
        
        return false;
    }
    
    
};

int main(int argc, const char * argv[]) {
    // insert code here...
    
    if(argc < 7)
    {
        cout << "#err! miss args! Usage: " << argv[0] << " [config file] [max error] [totle area] [house num] [max area] [min area]" << endl;
        return 1;
    }
    
    string strConfPath = argv[1];
    double fErrSize = atof(argv[2]);
    double fMyArea = atof(argv[3]);
    uint32_t u32MyNum = atoi(argv[4]);
    uint32_t u32MaxSize = atoi(argv[5]);
    uint32_t u32MinSize = atoi(argv[6]);
    
    vector<double> vecFilter;
    
    CMyHousePlan oHousePlan(strConfPath);
    oHousePlan.Done(fErrSize, fMyArea, u32MyNum, u32MaxSize, u32MinSize);
    
    
    return 0;
}
