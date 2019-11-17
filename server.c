//json.cpp
#include<iostream>
#include<json/json.h>

using namespace std;

int main()
{
    int       mark = 1;
    string    pwd("123456");

    //json的字符串格式
    string test="{\"id\":1,\"name\":\"kurama\"}";

    //reader用于将字符串格式转化为json
    Json::Reader  reader;
    //定义json变量
    Json::Value   value;

    if(reader.parse(test,value))//字符串转json
    {
        if(!value["id"].isNull())
        {
            cout<<value["id"].asInt()<<endl;
            //asString()用于转化为string,直接输出是带双引号的
            cout<<value["name"].asString()<<endl;
        }
    }

    Json::Value  json;
    //初始化json
    json["mark"] = Json::Value(mark);
    json["pass"] = Json::Value(pwd);

    cout<<json["mark"].asInt()<<json["pass"].asString()<<endl;

    string   s;
    //json转字符串
    s = json.toStyledString();

    cout<<s<<endl;
    Json::Value  js;
    reader.parse(s,js);
    cout<<js["pass"].asString()<<endl;

    //转化为不带格式的字符串
    Json::FastWriter writer;  
    std::string out = writer.write(js);  

}
