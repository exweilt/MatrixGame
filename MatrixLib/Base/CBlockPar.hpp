// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <vector>
#include <list>

#include "CMain.hpp"
#include "CHeap.hpp"
#include "CException.hpp"
#include "CBuf.hpp"
#include "Tracer.hpp"

namespace Base {

class CBlockPar;
class BPCompiler;

class ParamParser : public std::wstring
{
public:
    ParamParser(const std::wstring& str)
    : std::wstring{str}
    {
    }

    ParamParser() = default;
    ~ParamParser() = default;


    bool GetBool() const;
    int GetInt(void) const;
    DWORD GetDword(void) const;
    double GetDouble(void) const;
    int GetHex(void) const;
    DWORD GetHexUnsigned(void) const;

    bool IsOnlyInt(void) const;

    // Функции для работы с параметрами
    // Примеры :
    //      Str="count=5,7"    GetCountPar("=,")           return 3
    //      Str="count=5,7"    GetStrPar(str,1,"=")        str="5,7"
    //      Str="count=5,7"    GetStrPar(2,"=,").GetInt()  return 7

private:
    int GetSmePar(int np, const wchar *ogsim) const;
    int GetLenPar(int smepar, const wchar *ogsim) const;

public:
    int GetCountPar(const wchar *ogsim) const;
    ParamParser GetStrPar(int np, const wchar *ogsim) const {
        int sme = GetSmePar(np, ogsim);
        return std::wstring(c_str() + sme, GetLenPar(sme, ogsim));
    }

    ParamParser GetStrPar(int nps, int npe, const wchar *ogsim) const;
};

class BASE_API CBlockParUnit : public CMain {
    friend CBlockPar;
    friend BPCompiler;

private:
    CHeap *m_Heap;

    CBlockPar *m_Parent;

    int m_Type;  // 0-empty 1-par 2-block
    std::wstring m_Name;
    std::wstring m_Com;
    union {
        std::wstring *m_Par;
        CBlockPar *m_Block;
    };

    int m_FastFirst;  // Смещение до первого элемента с одинаковым именем
    int m_FastCnt;  // Количество с одинаковым именем. Инициализировано только для первого
public:
    CBlockParUnit(CHeap *heap = NULL);
    ~CBlockParUnit();

    void Clear(void);
    void ChangeType(int nt);

    void CopyFrom(CBlockParUnit &bp);
};

class BASE_API CBlockPar : public CMain {
    friend CBlockParUnit;
    friend BPCompiler;

private:
    std::list<CBlockParUnit*> m_Units;

    int m_CntPar;
    int m_CntBlock;

    std::wstring m_FromFile;
public:
    CBlockPar();
    ~CBlockPar();

    void Clear(void);

    void CopyFrom(CBlockPar &bp);

private:
    CBlockParUnit *UnitAdd(void);
    void UnitDel(CBlockParUnit *el);
    CBlockParUnit *UnitGet(const wchar *path, int path_len = -1);

    //////////////////////////////////////////////////////////////
private:
    bool ParSetNE(const std::wstring& name, const std::wstring& zn);
    const std::wstring* ParGetNE_(const std::wstring& name, int index) const;

public:
    CBlockParUnit *ParAdd(const std::wstring& name, const std::wstring& zn);

    void ParSet(const std::wstring &name, const std::wstring &zn) {
        if (!ParSetNE(name, zn))
            ERROR_E;
    }

    void ParSetAdd(const std::wstring &name, const std::wstring &zn) {
        if (!ParSetNE(name, zn))
            ParAdd(name, zn);
    }

    void ParSetAdd(const std::wstring& name, double value)
    {
        ParSetAdd(name, utils::format(L"%.8f", value));
    }


    void ParDelete(int no);


    ParamParser ParGet(const std::wstring& name, int index = 0) const
    {
        const std::wstring *str = ParGetNE_(name, index);
        if (str == NULL)
            ERROR_S2(L"Not found: ", name.c_str());
        return *str;
    }

    ParamParser ParGetNE(const std::wstring& name, int index = 0) const {
        const std::wstring *str = ParGetNE_(name, index);
        if (str != NULL)
            return *str;
        else
            return std::wstring();
    }

    int ParCount(void) const { return m_CntPar; }
    int ParCount(const std::wstring &name) const;

    ParamParser ParGet(int no) const;
    void ParSet(int no, const std::wstring &zn);
    ParamParser ParGetName(int no) const;

    //////////////////////////////////////////////////////////////
public:
    CBlockPar *BlockAdd(const std::wstring& name);
    CBlockPar *BlockGetNE(const std::wstring &name);
    CBlockPar *BlockGet(const std::wstring &name);

    CBlockPar *BlockGetAdd(const std::wstring &name);

    void BlockDelete(const std::wstring &name);
    void BlockDelete(int no);

    int BlockCount(void) const { return m_CntBlock; }
    int BlockCount(const std::wstring &name) const;

    CBlockPar *BlockGet(int no);
    const CBlockPar *BlockGet(int no) const;
    ParamParser BlockGetName(int no) const;

    //////////////////////////////////////////////////////////////
public:
    const std::wstring &ParPathGet(const std::wstring &path);
    void ParPathAdd(const std::wstring &path, const std::wstring &zn);
    void ParPathSet(const std::wstring &path, const std::wstring &zn);
    void ParPathSetAdd(const std::wstring &path, const std::wstring &zn);
    void ParPathDelete(const std::wstring &path);

    //////////////////////////////////////////////////////////////
public:
    CBlockPar *BlockPathGet(const std::wstring &path);
    CBlockPar *BlockPathAdd(const std::wstring &path);
    CBlockPar *BlockPathGetAdd(const std::wstring &path);

    //////////////////////////////////////////////////////////////
    int AllCount(void) { return m_Units.size(); }
    int AllGetType(int no);
    CBlockPar *AllGetBlock(int no);
    const std::wstring &AllGetPar(int no);
    const std::wstring &AllGetName(int no);

    //////////////////////////////////////////////////////////////

private:
    void SaveInText(CBuf &buf, bool ansi = false, int level = 0);

public:
    int LoadFromText(const std::wstring &text);
    void LoadFromTextFile(const std::wstring &filename);
    void SaveInTextFile(const std::wstring &filename, bool ansi = false);
};

}  // namespace Base