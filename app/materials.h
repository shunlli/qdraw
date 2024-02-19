#ifndef MATERIALS_H
#define MATERIALS_H

#include <QColor>

struct Material
{
    QString name;
    QColor color;
};

class Materials
{
private:
   Materials() = default;
   Materials(const Materials&) = delete;
public:
    static Materials* instance()
    {
        static Materials instance;

        // add some fake materials
        QStringList materialNames;
        materialNames << "SiO2" << "Silicon"
            << "Nickel" << "Oxide";
        QStringList colorNames;
        colorNames << "lightGray" << "green"
            << "cyan" << "yellow";
        for (int i=0; i<materialNames.count(); i++){
            
            Material mat;
            mat.name = materialNames[i];
            mat.color = QColor(colorNames[i]);
            instance.m_materials.append(mat);
        }
        return &instance;
    };

    QList<Material> getMaterials() {return m_materials; }
    QColor materialColor(QString name) {
        for (auto mat:m_materials)
        {
            if (mat.name == name)
                return mat.color;
        }
        return QColor();
    }
private:
    QList<Material> m_materials;
};

#endif // MATERIALS_H
