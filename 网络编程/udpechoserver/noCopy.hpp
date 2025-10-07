#pragma once

class noCopy
{
public:
    noCopy() = default;
    ~noCopy() = default;
    noCopy(const noCopy&) = delete;
    noCopy& operator=(const noCopy&) = delete;
};