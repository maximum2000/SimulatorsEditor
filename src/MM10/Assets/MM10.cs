/***************************************************************************
MM10.cs -  Расчет математической модели аппаратов
-------------------
begin                : 24 май 2023
copyright            : (C) 2023 by Гаммер Максим Дмитриевич (maximum2000)
email                : maxim.gammer@yandex.ru
***************************************************************************/

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using System.IO;
using UnityEngine.UI;

public class MM10 : MonoBehaviour
{
    private int maxx = 80;
    private int maxy = 80;


    public MyElementUI prefab;
    //public GameObject parent;
    public float dx;
    public float dy;

    public TextMeshProUGUI info_text;

    private List<MyElementUI> map2d;

    int editedCellIndex = -1;
    public GameObject CellEditor;
    public TMP_Dropdown CellEditorComponentNum;
    public TMP_InputField CellEditorM;
    public TMP_InputField CellEditorRo;
    public TMP_InputField CellEditorC;
    public TMP_InputField CellEditorQ;
    public TMP_InputField CellEditorType;


    public enum ModeType
    {
        none,
        wall,
        gas,
        fluid,
        solid,
        delete,
        edit
    }

    public ModeType selectedType;



    public void OneStep()
    {
        //идея:
        //зная объем ячейки определяем занятые объемы твердыми компонентами, потом жидкими... для этого нужна плотность
        //зная объем занимаемый газом, его массу.. теплоемкосьб газа и теплоту ... можем вычислить давление и температура газа
        //зная массу жидких и твердых компонентов ... и их теплоемкость и теплоту... можем вычислить и их температуры
        //потом осуществить теплоперенос и распределение теплоты для стабильного состояния (температура всех компонентов выравнивается)

        //идея:
        //зная векторную картину перемещений, можем оптимизировать порядок выборки координат для просчетов методом монтекарло
        //т.е. будем стараться чаще и вперед просчитывать области, активно отдающие массу для ее скорейшего распределения

        //!!!

        //0. Просто все ровно распределить, а именно
        //1. Если есть место снизу то скинуть туда твердое
        //   (скинуть все в порядке увеличения плотности в принципе)
        //2. Если есть место снизу то скинуть туда жидкость
        //3. Выровнять уровни слева\центр\справа для твердого и жидкого, т.е. перераспределить горизонтально
        //4. Считаем свободное пространство и если оно есть, то распределяем газ ровно по свободному объему

        //координаты центра свертки
        const int x = 27; //27
        const int y = 44; //44

        //свертка 3*3  ! только нечетные размеры
        const int width = 3;
        const int height = 3;
        int _w = (width - 1) / 2;
        int _h = (height - 1) / 2;
        //

        //суммирую вообще все 3*3 в 3 списка (твердое, жидкое, газ) вместе со всеми их энергиями и т.д.
        List<myComponent> summSolid = new List<myComponent>();
        List<myComponent> summFluid = new List<myComponent>();
        List<myComponent> summGas = new List<myComponent>();
        //счетчик объема
        double summV = 0;
        //
        for (int ix = x - _w; ix <= x + _w; ix++)
        {
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                int _index = ix * maxx + iy;
                //Debug.Log("ix=" + ix + " " + "iy=" + iy);
                //Debug.Log("index1=" + _index + " " + "index2="  + map2d[_index].index);
                //суммирую всю твердое и кидаю вниз
                bool isWall = false;
                if (map2d[_index].data.components.Count > 0)
                {
                    //прохожу все компоненты в этой клетке
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.gas)
                        {
                            summGas.Add(map2d[_index].data.components[i]);
                        }
                        else if (map2d[_index].data.components[i].type == myComponentType.fluid)
                        {
                            summFluid.Add(map2d[_index].data.components[i]);
                        }
                        else if (map2d[_index].data.components[i].type == myComponentType.solid)
                        {
                            summSolid.Add(map2d[_index].data.components[i]);
                        }

                        if (map2d[_index].data.components[i].type == myComponentType.wall)
                        {
                            isWall = true;
                        }
                    }
                }
                //запоминаю доступный объем клеток
                if (isWall == false)
                {
                    summV += map2d[_index].data.V;
                }
                //очищаю клетку от данных
                if (isWall == false)
                {
                    map2d[_index].data.Clear();
                }
                //
            }
        }
        //
        Debug.Log("summSolid.count=" + summSolid.Count);
        Debug.Log("summFluid.count=" + summFluid.Count);
        Debug.Log("summGas.count=" + summGas.Count);
        Debug.Log("summV=" + summV);
        //конец суммирования



        //проходим все 3 списка и сливаем "подобные с подобными" (сравнивая плотности) и переспределяя количество теплоты
        //т.е. из 3х записей про песок должна получится одна результирующая
        ////причем если две жидкости имеют одну плотность считаю жидкость одинаковой и "сливаю" все такие компоненты в "1" с учетом перераспределения энергии
        //т.е. сейчас плотность служит признаком вещества
        //берем первый элемент и идем сравнивать от последнего к первому, если сливаем в первый, то удаляем элемент
        {
            if (summSolid.Count > 0)
            {
                for (int f = 0; f < summSolid.Count; f++)
                {
                    for (int b = summSolid.Count - 1; b > f; b--)
                    {
                        //!!!! добавить плотность в сравнение
                        if (summSolid[f].type == summSolid[b].type)
                        {
                            summSolid[f].m += summSolid[b].m;
                            summSolid[f].Q += summSolid[b].Q;
                            summSolid.RemoveAt(b);
                        }
                    }
                }
            }

            if (summFluid.Count > 0)
            {
                for (int f = 0; f < summFluid.Count; f++)
                {
                    for (int b = summFluid.Count - 1; b > f; b--)
                    {
                        //!!!! добавить плотность в сравнение
                        if (summFluid[f].type == summFluid[b].type)
                        {
                            summFluid[f].m += summFluid[b].m;
                            summFluid[f].Q += summFluid[b].Q;
                            summFluid.RemoveAt(b);
                        }
                    }
                }
            }

            if (summGas.Count > 0)
            {
                for (int f = 0; f < summGas.Count; f++)
                {
                    for (int b = summGas.Count - 1; b > f; b--)
                    {
                        //!!!! добавить плотность в сравнение
                        if (summGas[f].type == summGas[b].type)
                        {
                            summGas[f].m += summGas[b].m;
                            summGas[f].Q += summGas[b].Q;
                            summGas.RemoveAt(b);
                        }
                    }
                }
            }


            Debug.Log("summSolid.count=" + summSolid.Count);
            //Debug.Log("summSolid.m=" + summSolid[0].m);
            Debug.Log("summFluid.count=" + summFluid.Count);
            Debug.Log("summGas.count=" + summGas.Count);
        }
        //конец слития


        //сортируем по плотности все списки
        {

        }




        //вычисляем объем с учетом равенства силы (давления) между газом/жидкостью в фиксированном объеме
        //summV - полный объем
        double Vsolid = 0;
        double C1 = 0;
        double C2 = 0;
        double Vfluid = 0;
        double Vgas = 0;

        double Ffluid = 0;
        double Fgas = 0;
        {
            // Vsolid = Summ (m/Ro)  - объем твердых не зависит от газа и воды
            // Fг=Fж на границе раздела,
            // Fг = Summ  (m*E) / Vг
            // Fж = k * Summ (m/Ro) / Vж
            // Fг = С1 / Vг
            // Fж = С2 / Vж
            // C1/Vг = C2/Vж
            // Vг = с1/c2 * Vж
            // Vж = (Vвсего - Vsoild) / (с1/с2+1) 
            // Vг = Vвсего - Vsoild - Vж

            //коэффициент сжатия жидкости общий
            double k = 1000000.0;

            for (int i = 0; i < summSolid.Count; i++)
            {
                Vsolid += summSolid[i].m / summSolid[i].Ro;
            }

            for (int i = 0; i < summGas.Count; i++)
            {
                C1 += summGas[i].m * summGas[i].Q;
            }

            for (int i = 0; i < summFluid.Count; i++)
            {
                C2 += k * summFluid[i].m / summFluid[i].Ro;
            }

            if (C2 > 0)
            {
                Vfluid = (summV - Vsolid) / (C1 / C2 + 1);
                Ffluid = C2 / Vfluid;
            }
            Vgas = summV - Vsolid - Vfluid;
            Fgas = C1 / Vgas;

            Debug.Log("summV=" + summV);
            Debug.Log("Vsolid=" + Vsolid);
            Debug.Log("Vfluid=" + Vfluid);
            Debug.Log("Vgas=" + Vgas);
            Debug.Log("Ffluid=" + Ffluid);
            Debug.Log("Fgas=" + Fgas);
        }





        //а теперь заполняю 3*3 сначала твердыми снизу ровно в один уровень, затем жидкими выше твердых в один уровень, затем оставшееся газом
        //не забывая про энергию, теплоемкость и т.д.
        {
            //1. Зная Vsolid и массу/плотность твердых - считаем сколько клеток мы заполним, сколько полных слоев, сколько неполных, заполняем
            //запоминаем полностью забитые клетки, в них уже ничего не добавить, клетки занимаем послойно, сначала одним твердым типом потом другим

            //пока есть нераспределенные твердые.....
            int currentY = y - _h;

            while (summSolid.Count > 0)
            {
                //иду снизу вверх слоями... 
                //для каждого слоя считаю сколько можно в него записать....

                //считаю сколько свободного места в этом слое.... потом
                //а) если полностью хватает места под распределение текущего компонента и еще остается, то распределяю по слою
                //   и удаляю компонент
                //   currentY не меняю
                //б) если полностью хватает места под распределение текущего компонента и место в слое закончилось,
                //   то распределяю по слою и удаляю компонент,
                //   поднимаю слой выше - меняю  currentY (currentY++ если еще есть нераспределенные компоненты...
                //   ...если нет места  - это не ошибка (это эффект сжатия)
                //в) если полностью занимаю весь слой текущим компонентом и еще компонент остается
                //   ,то распределяю слой полностью, вычитаю массу и энергию из компонента и 
                //   поднимаю слой выше - меняю  currentY (currentY++ если еще есть нераспределенные компоненты...
                //  ...если нет места  - это не ошибка (это эффект сжатия)
                //г) если места в слое вообще нет - пропускаю слой
                //   поднимаю слой выше - меняю  currentY (currentY++ если еще есть нераспределенные компоненты...
                //  ...если нет места  - это не ошибка (это эффект сжатия)

                //объем требуемый для распределения текущего компонента
                double Vcomponent = summSolid[0].m / summSolid[0].Ro;

                //доступный объем в этом слое=
                double freeLayerV = 0;
                //ячеек доступно в этом слое
                int availableCells = 0;
                for (int ix = x - _w; ix <= x + _w; ix++)
                {
                    int _index = ix * maxx + currentY;

                    bool isWall = false;
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.wall)
                        {
                            isWall = true;
                            break;
                        }
                    }

                    if (isWall == true) continue;
                    availableCells++;

                    freeLayerV += map2d[_index].data.V;
                    for (int i = 0; i<  map2d[_index].data.components.Count; i++) 
                    {
                        freeLayerV -= map2d[_index].data.components[i].m / map2d[_index].data.components[i].Ro;
                    }
                }

                Debug.Log("Vcomponent=" + Vcomponent);
                Debug.Log("freeLayerV=" + freeLayerV);
                Debug.Log("availableCells=" + availableCells);

                //все, прохожу еще раз по слою и распределяю компонент в соотношении доступных ячеек 

                if ((availableCells == 0)||(freeLayerV == 0))
                {
                    currentY++;
                    if (currentY > y + _h)
                    {
                        Debug.Log("! error 1 !");
                        break;
                    }
                    continue;
                }

                //if (freeLayerV >= Vcomponent)
                {
                    for (int ix = x - _w; ix <= x + _w; ix++)
                    {
                        int _index = ix * maxx + currentY;
                        //
                        myComponent temp = new myComponent();
                        temp.type = myComponentType.solid;
                        temp.m = summSolid[0].m / (float)availableCells;
                        temp.Ro = summSolid[0].Ro; 
                        temp.C = summSolid[0].C; 
                        temp.Q = summSolid[0].Q / (float)availableCells; 
                        map2d[_index].data.components.Add(temp);
                    }
                    summSolid.RemoveAt(0);

                    if (freeLayerV == Vcomponent) currentY++;
                    if (currentY > y + _h)
                    {
                        Debug.Log("! error 2 !");
                        break;
                    }

                    continue;
                }

                //if (freeLayerV < Vcomponent)
            }
            //конец распределения твердых


            //2. Зная Vfluid и массу жидкости и плотности жидкостей - считаем сколько клеток мы заполним, сколько полных слоев, сколько неполных, заполняем
            //запоминаем полностью забитые клетки, в них уже ничего не добавить
            //клетки занимаем постойно, сначала одной жидкостью потом другой, в соответствии с плотностью, т.е. сначала воду, потом нефть
            //и еще не забываем про давление, которое увеличивается на RoGH от верхних к нижним слоям / уменьшается на RoGH от нижних к верхним

            //Алгоритм улучидь до....1.Считать давление от жидкости вниз, в т.ч.с учетом давления газа сверху, т.е.сжимаемость
            //Ячейка может содержать больше массы чем влезет по обхему изза давления
            //При распределении жидкости сразу считать что давит вниз сильнее и туда перекидывать сразу больше мас

            //пока есть нераспределенная жидкость.....
            //currentY - на каком слое остановились на распределении твердых

            //распределяем сверху в низ (от границы газ-жидкость), т.к. в той точке мы знаем давление == давление газа
            //если газа нет, то отталкиваемся от давления жидкости при данной массе
            //Vgas - известен, объем уже занятый газом
            //Vsolid - известен, объем уже занятый твердыми
            //Vfluid - известен, объем жидкости который необходимо распределить
            //Fgas == Fgas - силы (давления) на верхней границе "газ/жидкость"

            /*
             * Алгоритм:
             * 1. идем от границы соприкосновения "жидкость/газ" (если газа нет, то с самого верха)
             * 2. Зная объем занятый твердыми снизу и газом сверху - найти слой, с которого начнем заполнение
             * 3. Идем по всем компонентам жидкости от менее плотных к более плотным и...
             *    набираем ровно столько массы, чтобы распределить в текущем слое, с учетом равенства давлений с газом (для первого слоя) или (если газа нет) то с "Ffluid"
             *    если слой заполнен, а масса компоненты еще остались, переходим к нижнему слою и увеличиваем давление на [mg] или [qgh], если qgh то отдельно по каждой плотности q1*g*h + q2*g*h + ....
            */

            /*
             * Алгоритм:
             * 1. идем от границы соприкосновения "жидкость/газ" (если газа нет, то с самого верха)
             * 2. Зная объем занятый твердыми снизу и газом сверху - найти слой, с которого начнем заполнение
             *    Находим количество слоев, которые необходимо заполнить
             *    Находим среднюю плотность жидкости (Rom)
             *    Находим коэффициенты распределения массы по слоям чтобы выполнялось условие 
             *    Fж0 = Fгаза
             *    Fж1 = k * Summ (m/Rom) / Vж + Rom*g*h
             *    Fж2 = k * Summ (m/Rom) / Vж
             *    ..
             *    для певрхено первого слоя
             *    k * Summ (m/Rom) / Vж  =  Fж0
             *    Fж1 == Fж2 - для последующих
             * 
             * 
             * 3. Идем по всем компонентам жидкости от менее плотных к более плотным и...
             *    набираем ровно столько массы, чтобы распределить в текущем слое, с учетом равенства давлений с газом (для первого слоя) или (если газа нет) то с "Ffluid"
             *    если слой заполнен, а масса компоненты еще остались, переходим к нижнему слою и увеличиваем давление на [mg] или [qgh], если qgh то отдельно по каждой плотности q1*g*h + q2*g*h + ....
            */



            while (summFluid.Count > 0)
            {
                summFluid.RemoveAt(0);
            }
            //конец распределения жидкости


            //3. Зная  Vgas, распределяем газ ровно по ячейкам и все, финиш
            //причем все газы в отличие от твердых и жидких распределяем равномерно по одним и темже клеткам

            //пока есть нераспределенные газы.....
            //currentY = y + _h;
            while (summGas.Count > 0)
            {
                //заранее запоминаем сколько массы и энергии компонента мы должны сбросить на еденицу объема
                double massPerVolume = summGas[0].m / Vgas;
                double QPerVolume = summGas[0].Q / Vgas;
                bool done = false;
                //
                for (int ix = x - _w; ix <= x + _w; ix++)
                {
                    for (int iy = y + _h; iy >= y - _h; iy--)
                    {
                        int _index = ix * maxx + iy;

                        bool isWall = false;
                        for (int i = 0; i < map2d[_index].data.components.Count; i++)
                        {
                            if (map2d[_index].data.components[i].type == myComponentType.wall)
                            {
                                isWall = true;
                                break;
                            }
                        }

                        if (isWall == true) continue;

                        //итак, есть ячейка, у нее есть объем, туда мы закидываем massPerVolume * на объем ячейки 
                        {
                            myComponent temp = new myComponent();
                            temp.type = myComponentType.gas;
                            temp.m = (float)massPerVolume * map2d[_index].data.V;
                            temp.Ro = summGas[0].Ro;
                            temp.C = summGas[0].C;
                            temp.Q = (float)QPerVolume * map2d[_index].data.V; 
                            map2d[_index].data.components.Add(temp);
                        }

                        //убираем распределенный кусок массы
                        summGas[0].m -= (float)massPerVolume * map2d[_index].data.V;
                        //если все распределили, удаляем компонент и переходим к следующему
                        if (summGas[0].m < massPerVolume/10f)
                        {
                            done = true;
                            summGas.RemoveAt(0);
                            break;
                        }

                    }
                    if (done == true) break;
                }
                //
            }
            //конец распределения газа

        }




        //отрисовка
        for (int ix = x - _w; ix <= x + _w; ix++)
        {
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                int _index = ix * maxx + iy;
                map2d[_index].ManualUpdate();
            }
        }


        //Конец OneStep
    }

    public void CellClick(ref MyElementUI cell)
    {
        if (selectedType == ModeType.none)
        {
            //cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 0f, 0);
        }
        else if (selectedType == ModeType.gas)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.gas;
            temp.m = 1f;
            temp.Ro = 1.2754f; //температуре 0 °С, давлении 100 кПа, нулевой влажности
            temp.C = 1010f; //В интервале от -50 до 120°С ее величина практически не меняется — в этих условиях средняя теплоемкость воздуха равна 1010 Дж/(кг·град).
            temp.Q = temp.C * temp.m * 273.15f;  //Дж
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.fluid)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.fluid;
            temp.m = 1f;
            temp.Ro = 1000f; //кг\м3
            temp.C = 4180.6f; //Удельная теплоёмкость воды, Дж/(кг·K)
            temp.Q = temp.C * temp.m * 273.15f;  //Дж
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.solid)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.solid;
            temp.m = 1f;
            temp.Ro = 1700; //кг\м3
            temp.C = 835f; //Удельная теплоёмкость воды, Дж/(кг·K)
            temp.Q = temp.C * temp.m * 273.15f;  //Дж
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.wall)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.wall;
            temp.m = 1f;
            temp.Ro = 7800f; //кг\м3
            temp.C = 460f; //Удельная теплоёмкость воды, Дж/(кг·K)
            temp.Q = temp.C * temp.m * 273.15f;  //Дж
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.delete)
        {
            cell.data.Clear();
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.edit)
        {
            editedCellIndex = cell.index;
            CellEditor.SetActive(true);
            EditorShowComponentValues(0);
        }


    }


    public void EditorShowComponentValues(int z)
    {
        if (editedCellIndex == -1) return;

        CellEditorComponentNum.options.Clear();

        if (map2d[editedCellIndex].data.components.Count > z)
        {
            List<string> all = new List<string>();
            for (int i = 0; i < map2d[editedCellIndex].data.components.Count; i++)
            {
                all.Add(i.ToString("N0"));
            }
            CellEditorComponentNum.AddOptions(all);
            CellEditorComponentNum.SetValueWithoutNotify(z);
            
            CellEditorM.text = map2d[editedCellIndex].data.components[z].m.ToString();
            CellEditorRo.text = map2d[editedCellIndex].data.components[z].Ro.ToString();
            CellEditorC.text = map2d[editedCellIndex].data.components[z].C.ToString();
            CellEditorQ.text = map2d[editedCellIndex].data.components[z].Q.ToString();

            string temp = "";
            temp += map2d[editedCellIndex].data.components[z].type;            
            CellEditorType.text = temp;
        }
    }

    public void EditorComponentChange()
    {
        if (editedCellIndex == -1) return;
        int z = CellEditorComponentNum.value;

        if (map2d[editedCellIndex].data.components.Count > z)
        {
            map2d[editedCellIndex].data.components[z].m = float.Parse(CellEditorM.text);
            map2d[editedCellIndex].data.components[z].Ro = float.Parse(CellEditorRo.text);
            map2d[editedCellIndex].data.components[z].C = float.Parse(CellEditorC.text);
            map2d[editedCellIndex].data.components[z].Q = float.Parse(CellEditorQ.text);
        }
    }

    public void CellInfo(ref MyElementUI cell)
    {
        string text = "R:\t" + cell.gameObject.GetComponent<SpriteRenderer>().color.r.ToString("n4").Replace(",", ".") + System.Environment.NewLine;
        text += "G:\t" + cell.gameObject.GetComponent<SpriteRenderer>().color.g.ToString("n4").Replace(",", ".") + System.Environment.NewLine;
        text += "B:\t" + cell.gameObject.GetComponent<SpriteRenderer>().color.b.ToString("n4").Replace(",", ".") + System.Environment.NewLine;

        int xx = cell.index / maxx;
        int index_y = cell.index - (xx * maxx);
        int index_x = xx;
        text += "x:" + index_x + "\ty:" + index_y + System.Environment.NewLine; ;

        //cell.data
        text += "V:\t" + cell.data.V.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToUp:\t\t" + cell.data.transferToUp.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToDown:\t\t" + cell.data.transferToDown.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToLeft:\t\t" + cell.data.transferToLeft.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToRight:\t\t" + cell.data.transferToRight.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToUpLeft:\t\t" + cell.data.transferToUpLeft.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToUpRight:\t\t" + cell.data.transferToUpRight.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToDownLeft:\t" + cell.data.transferToDownLeft.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
        text += "transferToDownRight:\t" + cell.data.transferToDownRight.ToString("n8").Replace(",", ".") + System.Environment.NewLine;

        text += "Count:\t" + cell.data.components.Count.ToString("N0").Replace(",", ".") + System.Environment.NewLine;

        if (cell.data.components.Count>0)
        {
            for (int i=0; i < cell.data.components.Count; i++)
            {
                text += System.Environment.NewLine;
                text += "new component num:\t" + i.ToString("N0") + System.Environment.NewLine;
                text += "type:\t" + cell.data.components[i].type + System.Environment.NewLine;
                text += "m:\t" + cell.data.components[i].m.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "C:\t" + cell.data.components[i].C.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "Q:\t" + cell.data.components[i].Q.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "Ro:\t" + cell.data.components[i].Ro.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
            }
        }

        info_text.text = text;
    }
    

    // Start is called before the first frame update
    void Start()
    {
        selectedType = ModeType.none;

        NewTable();
    }

    public void ClearTable()
    {
        CellEditor.SetActive(false);
        for (int t = 0; t < maxx * maxy; t++)
        {
            map2d[t].data.Clear();
            map2d[t].ManualUpdate();
        }
    }

    public void LoadScene()
    {
        CellEditor.SetActive(false);

        string filename = Path.Combine(Application.streamingAssetsPath, "test.model");
        if (File.Exists(filename) == false) return;


        //NewTable();
        ClearTable();


        BinaryReader reader;
        reader = new BinaryReader(File.Open(filename, FileMode.Open));

        int i = 0;

        while (reader.BaseStream.Position != reader.BaseStream.Length)
        {
            try
            {
                map2d[i].data.Clear();
                map2d[i].data.V = reader.ReadSingle();
                map2d[i].data.transferToUp = reader.ReadSingle();
                map2d[i].data.transferToDown = reader.ReadSingle();
                map2d[i].data.transferToLeft = reader.ReadSingle();
                map2d[i].data.transferToRight = reader.ReadSingle();
                map2d[i].data.transferToUpLeft = reader.ReadSingle();
                map2d[i].data.transferToUpRight = reader.ReadSingle();
                map2d[i].data.transferToDownLeft = reader.ReadSingle();
                map2d[i].data.transferToDownRight = reader.ReadSingle();

                int count = reader.ReadInt32();
                for (int y = 0; y < count; y++)
                {
                    myComponent temp = new myComponent();
                    temp.type = (myComponentType)reader.ReadInt32();
                    temp.m = reader.ReadSingle();
                    temp.C = reader.ReadSingle();
                    temp.Q = reader.ReadSingle();
                    temp.Ro = reader.ReadSingle();
                    map2d[i].data.components.Add(temp);
                }

                map2d[i].ManualUpdate();
                i++;
            }
            catch (EndOfStreamException e)
            {

            }
            catch (System.ObjectDisposedException e)
            {

            }
        }
        reader.Close();
    }

    public void SaveScene()
    {
        if (map2d == null) return;

        BinaryWriter writer;
        string filename = Path.Combine(Application.streamingAssetsPath, "test.model");
        writer = new BinaryWriter(File.Open(filename, FileMode.Create));

        for (int i = 0; i < maxx * maxy; i++)
        {
            //writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.r);
            //writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.g);
            //writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.b);

            //V
            writer.Write(map2d[i].data.V);
            //transfer
            writer.Write(map2d[i].data.transferToUp);
            writer.Write(map2d[i].data.transferToDown);
            writer.Write(map2d[i].data.transferToLeft);
            writer.Write(map2d[i].data.transferToRight);
            writer.Write(map2d[i].data.transferToUpLeft);
            writer.Write(map2d[i].data.transferToUpRight);
            writer.Write(map2d[i].data.transferToDownLeft);
            writer.Write(map2d[i].data.transferToDownRight);
            //count
            writer.Write(map2d[i].data.components.Count);
            //
            if (map2d[i].data.components.Count > 0)
            {
                for (int y = 0; y < map2d[i].data.components.Count; y++)
                {
                    writer.Write((int)map2d[i].data.components[y].type);
                    writer.Write(map2d[i].data.components[y].m);
                    writer.Write(map2d[i].data.components[y].C);
                    writer.Write(map2d[i].data.components[y].Q);
                    writer.Write(map2d[i].data.components[y].Ro);
                }
            }
        }
        writer.Close();
    }



    public void SelectGas()
    {
        selectedType = ModeType.gas;
    }

    public void SelectFluid()
    {
        selectedType = ModeType.fluid;
    }

    public void SelectSolid()
    {
        selectedType = ModeType.solid;
    }

    public void SelectWall()
    {
        selectedType = ModeType.wall;
    }

    public void SelectNone()
    {
        selectedType = ModeType.none;
    }

    public void SelectDelete()
    {
        selectedType = ModeType.delete;
    }

    public void SelectEdit()
    {
        selectedType = ModeType.edit;
    }




    private void NewTable()
    {
        prefab.gameObject.SetActive(true);


        //чистим
        /*
        if (map2d != null)
        {
            for (int i = 0; i < maxx * maxy; i++)
            {
                Destroy(map2d[i].gameObject);
            }
            map2d.Clear();
        }
        */


        //создаем
        map2d = new List<MyElementUI>();

        for (int x = 0; x < maxx; x++)
        {
            for (int y = 0; y < maxy; y++)
            {
                MyElementUI myclone = Instantiate(prefab);
                myclone.gameObject.transform.localPosition = new Vector3(dx + x * 0.11f, dy + y * 0.11f, 0);
                //myclone.gameObject.GetComponent<SpriteRenderer>().color = new Color((float)x/40f, (float)y/40f, 0);
                // myclone.transform.parent = parent.transform;
                myclone.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 1f, 1f);
                int index = x * maxx + y;
                myclone.index = index;


                myclone.data = new myElementData();
                myclone.data.Clear();

                map2d.Add( myclone );
            }
        }
        prefab.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
