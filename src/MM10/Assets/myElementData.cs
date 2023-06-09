/***************************************************************************
myElementData.cs -  опичание ячейки модели
-------------------
begin                : 24 май 2023
copyright            : (C) 2023 by Гаммер Максим Дмитриевич (maximum2000)
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
    //Теплоёмкость — количество теплоты, поглощаемой (выделяемой) телом в процессе нагревания (остывания) на 1 кельвин
    //удельной теплоёмкости (теплоёмкости единицы массы тела)
    //У нас - удельная теплоемкость, т.е. теплоёмкость единицы массы тела (килограмма)
    public float C;
    //Теплота (энергия) , джоуль
    public float Q;
    //плотность (для жидкостей и твердых тел)
    //плотность — скалярная физическая величина, определяемая как отношение массы тела к занимаемому этим телом объёму, кг/м3
    public float Ro;

    //идея:
    //зная объем ячейки определяем занятые объемы твердыми компонентами, потом жидкими... для этого нужна плотность
    //зная объем занимаемый газом, его массу.. теплоемкосьб газа и теплоту ... можем вычислить давление и температура газа
    //зная массу жидких и твердых компонентов ... и их теплоемкость и теплоту... можем вычислить и их температуры
    //потом осуществить теплоперенос и распределение теплоты для стабильного состояния (температура всех компонентов выравнивается)

    
}

public class myElementData
{
    //перечень компонентов в этой ячейке
    public List<myComponent> components;
    //объем ячейки, м3
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

    //идея:
    //зная векторную картину перемещений, можем оптимизировать порядок выборки координат для просчетов методом монтекарло
    //т.е. будем стараться чаще и вперед просчитывать области, активно отдающие массу для ее скорейшего распределения


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


