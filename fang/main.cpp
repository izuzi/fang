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
    CZLoad* m_pConf;
    bool bInitFlag;
    CMyHouseType oHouseType;
    
    double fErrSize;
    double fMySize;
    uint32_t u32MyNum;
    
    vector< vector<double> > vecResult;

    
public:
    CMyHousePlan(string strConf) : m_pConf(NULL), bInitFlag(false)
    {
        Init(strConf);
    }
    
    ~CMyHousePlan()
    {
        if(m_pConf) delete m_pConf;
        m_pConf = NULL;
    }
    
    void Done(double _fErrSize, double _fMySize, uint32_t _u32MyNum, uint32_t u32MaxSize, uint32_t u32MinSize)
    {
        if(!bInitFlag) return;
        
        Store(_fErrSize, _fMySize, _u32MyNum);
        Choose();
        Order();
        Show(u32MaxSize, u32MinSize);
    }
    
private:
    bool Init(string strConf)
    {
        m_pConf = new CZLoad(strConf);
        
        vector< vector<string> > vecConfData;
        if(false == m_pConf->Read(vecConfData))
        {
            bInitFlag = false;
            return false;
        }
        
        for(int i=0; i<vecConfData.size(); i++)
        {
            if(vecConfData[i].size() < 2)
            {
                return false;
            }
            
            double fSize = atof(vecConfData[i][0].c_str());
            string strPos = vecConfData[i][1];
            
            oHouseType.Add(fSize, strPos);
        }
        
        bInitFlag = true;
        
        return true;
    }
    
    void Store(double _fErrSize, double _fMySize, uint32_t _u32MyNum)
    {
        fErrSize = _fErrSize;
        fMySize = _fMySize;
        u32MyNum = _u32MyNum;
    }
    
    void Choose()
    {
        vecResult.clear();
        
        for(int i=oHouseType.Size() - 1; i>=0; i--)
        {
            uint32_t u32HouseNum = 0;
            double fSumSize = 0;
            vector<double> vecPlan;
            
            GetOne(i, u32HouseNum, fSumSize,vecPlan);
        }
    }
    
    int GetOne(int i, uint32_t u32HouseNum, double fSumSize, vector<double> vecPlan)
    {
        u32HouseNum++;
        fSumSize += oHouseType.Index(i);
        vecPlan.push_back(oHouseType.Index(i));

        
        if(i < 0)
        {
            return 2;
        }
        
        if(u32HouseNum > u32MyNum)
        {
            return 1;
        }
        
        if(u32HouseNum == u32MyNum && fabs(fMySize - fSumSize) <= fErrSize )
        {
            vecPlan.push_back(fMySize - fSumSize);
            vecResult.push_back(vecPlan);
            return 0;
        }
        
        for(int j=i; j>=0; j--)
        {
            GetOne(j, u32HouseNum, fSumSize, vecPlan);
        }
        
        
        return -1;
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
    double fErrSize=atof(argv[2]);
    double fMySize=atof(argv[3]);
    uint32_t u32MyNum=atoi(argv[4]);
    uint32_t u32MaxSize=atoi(argv[5]);
    uint32_t u32MinSize=atoi(argv[6]);
    
    vector<double> vecFilter;
    
    CMyHousePlan oHousePlan(strConfPath);
    oHousePlan.Done(fErrSize, fMySize, u32MyNum, u32MaxSize, u32MinSize);
    
    
    return 0;
}

