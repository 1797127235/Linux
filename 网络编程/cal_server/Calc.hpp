#pragma once

#include<iostream>
#include"Protocol.hpp"

class Calc
{
public:
    Calc(){}
    ~Calc(){}

    static std::shared_ptr<Response> Process(const Request& req)
    {
        char op = req._op;
        int x = req._x;
        int y = req._y;
        int result = 0;
        switch (op)
        {
        case '+':
            result = x + y;
            break;
        case '-':
            result = x - y;
            break;
        case '*':
            result = x * y;
            break;
        case '/':
            if(y == 0)
            {
                return std::make_shared<Response>(0,1);
            }
            result = x / y;
            break;
        case '%':
            if(y == 0)
            {
                return std::make_shared<Response>(0,1);
            }
            result = x % y;
            break;
        default:
            return std::make_shared<Response>(0,1);
            break;
        }
        return std::make_shared<Response>(result,0);
    }
};