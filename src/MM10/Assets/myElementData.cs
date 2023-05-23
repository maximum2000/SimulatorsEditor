/***************************************************************************
myElementData.cs -  опичание €чейки модели
-------------------
begin                : 24 май 2023
copyright            : (C) 2023 by √аммер ћаксим ƒмитриевич (maximum2000)
email                : maxim.gammer@yandex.ru
***************************************************************************/

using System.Collections;
using System.Collections.Generic;


public enum myComponentType
{
    none,
    wall,
    gas,
    fluid,
    solid
}

/*
public enum myComponentTypeDetail
{
    none,
    steel,
    methan,
    o2,
    water,
    oil,
    sand,
    concrete
}
*/


public class myComponent
{
    //масса компонента
    public float m=0;
    //тип компонента
    public myComponentType type = myComponentType.none;
    //public myComponentTypeDetail typeDetail = myComponentTypeDetail.none;
    //теплоемкость
    //“еплоЄмкость Ч количество теплоты, поглощаемой (выдел€емой) телом в процессе нагревани€ (остывани€) на 1 кельвин
    //удельной теплоЄмкости (теплоЄмкости единицы массы тела)
    //” нас - удельна€ теплоемкость, т.е. теплоЄмкость единицы массы тела (килограмма)
    public float C;
    //“еплота (энерги€) , джоуль
    public float Q;
    //плотность (дл€ жидкостей и твердых тел)
    //плотность Ч скал€рна€ физическа€ величина, определ€ема€ как отношение массы тела к занимаемому этим телом объЄму, кг/м3
    public float Ro;

    //иде€:
    //зна€ объем €чейки определ€ем зан€тые объемы твердыми компонентами, потом жидкими... дл€ этого нужна плотность
    //зна€ объем занимаемый газом, его массу.. теплоемкосьб газа и теплоту ... можем вычислить давление и температура газа
    //зна€ массу жидких и твердых компонентов ... и их теплоемкость и теплоту... можем вычислить и их температуры
    //потом осуществить теплоперенос и распределение теплоты дл€ стабильного состо€ни€ (температура всех компонентов выравниваетс€)

    
}

public class myElementData
{
    //перечень компонентов в этой €чейке
    public List<myComponent> components;
    //объем €чейки, м3
    public float V=1f;

    //вектор перехода массы их этого компонента [по схеме q9]
    public float transferToUp = 0;
    public float transferToDown = 0;
    public float transferToLeft = 0;
    public float transferToRight = 0;
    public float transferToUpLeft = 0;
    public float transferToUpRight = 0;
    public float transferToDownLeft = 0;
    public float transferToDownRight = 0;

    //иде€:
    //зна€ векторную картину перемещений, можем оптимизировать пор€док выборки координат дл€ просчетов методом монтекарло
    //т.е. будем старатьс€ чаще и вперед просчитывать области, активно отдающие массу дл€ ее скорейшего распределени€


    public void Clear()
    {
        components = new List<myComponent>();
        V = 1f;
        transferToUp = 0f;
        transferToDown = 0;
        transferToLeft = 0;
        transferToRight = 0;
        transferToUpLeft = 0;
        transferToUpRight = 0;
        transferToDownLeft = 0;
        transferToDownRight = 0;
    }
}


