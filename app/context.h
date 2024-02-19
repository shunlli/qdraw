#ifndef CONTEXT_H
#define CONTEXT_H

#include "materials.h"

enum BooleanOperation
{
    MergeOper,
    NewReplacesOldOper,
    OldReplacesNewOper,
    NewOverlpasOldOper,
    OldOverlpasNewOper
};

class Context
{
private:
   Context() = default;
   Context(const Context&) = delete;
public:
    static Context* instance()
    {
        static Context instance;
        return &instance;
    };

    QString currentMaterial() {return m_currentMaterial; }
    void setCurrentMaterial(QString mat) { m_currentMaterial = mat; }
    BooleanOperation boolOperation() {return m_boolOperation; }
    void setBooleanOperation(BooleanOperation oper) { m_boolOperation = oper; }
private:
    QString m_currentMaterial;
    BooleanOperation m_boolOperation{NewReplacesOldOper};
};

#endif // CONTEXT_H
