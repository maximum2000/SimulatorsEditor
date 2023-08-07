/***************************************************************************
MM10_MM.cs -  Расчет математической модели аппаратов
-------------------
begin                : 24 май 2023
copyright            : (C) 2023 by Гаммер Максим Дмитриевич (maximum2000)
email                : maxim.gammer@yandex.ru
***************************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial  class MM10 : MonoBehaviour
{
    //
    private List<myComponent> summSolid;
    private List<myComponent> summFluid;
    private List<myComponent> summGas;
    //счетчик объема
    private double summV = 0;
    private double Vsolid = 0;
    private double Vfluid = 0;
    private double Vgas = 0;
    //коэффициент сжатия жидкости общий
    private double k = 1000000000;//1000000.0;
    private int currentY = 0;
    private List<double> freeLayerCount;


    private void Step1(int x, int y, int _w, int _h)
    {
        //суммирую вообще все 3*3 в 3 списка (твердое, жидкое, газ) вместе со всеми их энергиями и т.д.
        summSolid = new List<myComponent>();
        summFluid = new List<myComponent>();
        summGas = new List<myComponent>();
        //счетчик объема
        summV = 0;
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
                    //проверяю не стена ли это
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.wall)
                        {
                            isWall = true;
                            break;
                        }
                    }
                    if (isWall == true) continue;

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
                    }
                }
                //запоминаю доступный объем клеток
                if (isWall == false)
                {
                    summV += map2d[_index].data.V;
                    //очищаю клетку от данных
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
    }

    private void Step2()
    {
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
                        if (summFluid[f].type == summFluid[b].type && (float)summFluid[f].Ro==(float)summFluid[b].Ro)
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
                        if (summGas[f].type == summGas[b].type && (float)summGas[f].Ro==(float)summGas[b].Ro)
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
            //Сортируем жидкости
            {
                myComponent temp;
                for (int i = 0; i < summFluid.Count; i++)
                {
                    for (int j = i + 1; j < summFluid.Count; j++)
                    {
                        if (summFluid[i].Ro > summFluid[j].Ro)
                        {
                            temp = summFluid[i];
                            summFluid[i] = summFluid[j];
                            summFluid[j] = temp;
                        }                   
                    }            
                }
            }
            //Сортируем газы
            {
                myComponent temp;
                for (int i = 0; i < summGas.Count; i++)
                {
                    for (int j = i + 1; j < summGas.Count; j++)
                    {
                        if (summGas[i].Ro > summGas[j].Ro)
                        {
                            temp = summGas[i];
                            summGas[i] = summGas[j];
                            summGas[j] = temp;
                        }
                    }
                }
            }
        }
    }


    private void Step3()
    {
        //вычисляем объем с учетом равенства силы (давления) между газом/жидкостью в фиксированном объеме
        //summV - полный объем
         Vsolid = 0;
        double C1 = 0;
        double C2 = 0;
         Vfluid = 0;
         Vgas = 0;

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


    }


    private void Step4(int x, int y, int _w, int _h)
    {
        //а теперь заполняю 3*3 сначала твердыми снизу ровно в один уровень, затем жидкими выше твердых в один уровень, затем оставшееся газом
        //не забывая про энергию, теплоемкость и т.д.
        {
            //1. Зная Vsolid и массу/плотность твердых - считаем сколько клеток мы заполним, сколько полных слоев, сколько неполных, заполняем
            //запоминаем полностью забитые клетки, в них уже ничего не добавить, клетки занимаем послойно, сначала одним твердым типом потом другим

            //пока есть нераспределенные твердые.....
            currentY = y - _h;

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
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        freeLayerV -= map2d[_index].data.components[i].m / map2d[_index].data.components[i].Ro;
                    }
                }

                Debug.Log("Vcomponent=" + Vcomponent);
                Debug.Log("freeLayerV=" + freeLayerV);
                Debug.Log("availableCells=" + availableCells);

                //все, прохожу еще раз по слою и распределяю компонент в соотношении доступных ячеек 

                if ((availableCells == 0) || (freeLayerV == 0))
                {
                    currentY++;
                    if (currentY > y + _h)
                    {
                        Debug.Log("! error 1 !");
                        break;
                    }
                    continue;
                }

                //Доработать, тут сейчас все в 1 слой валится, что неправильно
                //Вроде доработал.
                
                for (int ix = x - _w; ix <= x + _w; ix++)
                {
                    if (availableCells == 0 || freeLayerV == 0)
                    {
                        currentY++;
                        if (currentY > y + _h)
                        {
                            Debug.Log("! error 1 !");
                            break;
                        }
                        continue;
                    }
                    int _index = ix * maxx + currentY;
                    //если это не стена (там или чисто или стена)
                    if (map2d[_index].data.components.Count>0) continue;
                    //если это не стена, тогда заливаем
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.solid;
                    if (((summSolid[0].m/summSolid[0].Ro) / (float)availableCells) <= map2d[_index].data.V)
                        temp.m = summSolid[0].m / (float)availableCells;
                    else
                        temp.m = map2d[_index].data.V * summSolid[0].Ro;
                    temp.Ro = summSolid[0].Ro;
                    temp.C = summSolid[0].C;
                    temp.Q = (temp.m / summSolid[0].m) * summSolid[0].Q;
                    //temp.Q = summSolid[0].Q / (float)availableCells; ЗАМЕНИЛ СТРОКОЙ ВЫШЕ
                    map2d[_index].data.components.Add(temp);
                    //Добавил вычитание из общей массы чтоб адекватнее делалось 
                    summSolid[0].m -= temp.m;
                    summSolid[0].Q -= temp.Q;
                    availableCells--;
                }
                //Добавил проверку, что все распределили, если нет, то переходим на след уровень.
                if (summSolid[0].m == 0f)
                    summSolid.RemoveAt(0);
                else currentY++;
                //if (freeLayerV == Vcomponent) currentY++;
                if (currentY > y + _h)
                {
                    Debug.Log("! error 2 !");
                    break;
                }
            }
            //конец распределения твердых
        }
    }

    double summM_fluid = 0;
    private void Step5(int x, int y, int _w, int _h)
    {
        //2. Заполняем жидеость, зная Vfluid и массу жидкостей и плотности жидкостей
        //клетки занимаем постойно, сначала одной жидкостью потом другой, в соответствии с плотностью, т.е. сначала воду, потом нефть
        //и еще не забываем про давление, которое увеличивается на RoGH от верхних к нижним слоям / уменьшается на RoGH от нижних к верхним

        //Дано:
        //распределяем сверху в низ (от границы газ-жидкость), т.к. в той точке мы знаем давление == давление газа
        //если газа нет, то отталкиваемся от давления жидкости при данной массе
        //Vgas - известен, объем уже занятый газом
        //Vsolid - известен, объем уже занятый твердыми
        //Vfluid - известен, объем жидкости который необходимо распределить
        //Fgas == Fgas - силы (давления) на верхней границе "газ/жидкость"
        //currentY - на каком слое остановились на распределении твердых

        /*
         * Алгоритм:
         *  ! поскольку ранее мы считали усредненную жидкость (С2/V) , то точно на  Fгаза == Fпервого_слоя == ... == Fпоследнего_слоя - мы не выйдем, масса на объем взята в среднем
         *  ! и если мы будем танцевать от газа то у нас массы не хватит на последние слои, т.к. потратим массы больше чем среднее, если танцевать снизу то аналогично полуим меньшее давление чем нужно
         *  ! поэтому не заморачиваемся и просто распределяем массы жидкости не смотря на давление газа на первый слой....
         *  
         * 1. идем от границы соприкосновения "жидкость/газ" (если газа нет, то с самого верха)
         * 2. Зная объем занятый твердыми снизу и газом сверху - найти слой, с которого начнем заполнение
         * 3. Находим количество слоев, которые необходимо заполнить
         * 4. Находим среднюю плотность жидкости (Rom)
         * 5. Находим коэффициенты распределения массы по слоям чтобы выполнялось условие , да, игнорируем на давление газа
         *    Fж1 = k * Summ (m/Rom) / Vж + Rom*g*h  [низ верхнего слоя]
         *    Fж2 = k * Summ (m/Rom) / Vж            [верх нижнего слоя]
         *    Fж1 == Fж2 
         *              
         * 6. Идем по всем компонентам жидкости от менее плотных к более плотным и...
         *    набираем ровно столько массы, сколько нашли ранее на шаге 5
         *    
         *    
         *    еще проще.... находим колько слоев будет занято жидкостью, далее 3 сценария
         *    1 слой. Все понятно, распределяем все компоненты жидкости в этот слой, все
         *    2 слоя. Считаем сколько свободный объем для жидкости в верхнем слое, сколько в нижнем, составляем уравнение Fж1 == Fж2  и распределяем
         *    3 слоя. Считаем сколько свободного объема в верхнем, в среднем и в нижнем... состаляем уравнение Fж1 == Fж2 == Fж3 и распределяем
         *
         *    Для случая 2
         *    или через + Rom*g*h1    (столб жидкости)
         *    k * m1 / Rom / V1 + Rom*g*h1    == k * m2 / Rom / V2 
         *    или через + m*g         (масса вниз)
         *    k * (m1/Rom)/V1 + m1*g   == k* (m2/Rom)/V2
         *    m1 = (m_summ/V2) / (1/V1 + 1/V2 + g*Rom/k)
         *    
         *    Для случая 3
         *    m1 = m_summ * (v1/V_summ)
         *    m2 = m_summ * (v2/V_summ)
         *    m3 = m_summ * (v3/V_summ)
         *    
         *    minusM1 = m1*0.02;
         *    m1 -= minusM1
         *    m2 += minusM1
         *    minusM2 = m2*0.01
         *    m2 -= minusM2
         *    m3 += minusM2
        */
        freeLayerCount = new List<double>();
        if (summFluid.Count > 0)
        {
            //рассчитываю среднюю плотность (как массу общую на объем общий)
            summM_fluid = 0;
            for (int i = 0; i < summFluid.Count; i++)
            {
                summM_fluid += (double)summFluid[i].m;
            }
            Debug.Log("summM_fluid=" + summM_fluid);
            Debug.Log("Vfluid=" + Vfluid);
            double avgRo = summM_fluid / Vfluid;
            Debug.Log("avgRo=" + avgRo);

            //считаю количество слоев
            //начинаю с слоя где закончились твердые компоненты и иду наверх считая свободный объем, пока не закончится....
            //запоминаю доступный объем в каждом слое
            //количество элементов в массиве - количество слоев

            //распределение объемов по слоям
            
            //currentY - на каком слое остановились на распределении твердых
            bool isStop = false;
            double VfluidCounterSumm = 0;
            int currentY_old = currentY;
            while (isStop == false)
            {
                double VfluidCounter = 0;
                for (int ix = x - _w; ix <= x + _w; ix++)
                {
                    int _index = ix * maxx + currentY;

                    //если стена
                    bool isWall = false;
                    //Debug.Log("Count="+map2d.Count+"; index="+_index);
                    //Debug.Log("x="+ix+"; y="+currentY);
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.wall)
                        {
                            isWall = true;
                            break;
                        }
                    }
                    if (isWall == true) continue;

                    //считаем суммарный доступынй  объем
                    VfluidCounter += map2d[_index].data.V;
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.solid)
                        {
                            VfluidCounter -= map2d[_index].data.components[i].m / map2d[_index].data.components[i].Ro;
                        }
                    }
                }

                if (VfluidCounterSumm + VfluidCounter >= Vfluid - 0.000001)
                {
                    isStop = true;
                    freeLayerCount.Add(Vfluid - VfluidCounterSumm);
                    break;
                }
                else
                {
                    //Debug.Log("VfluidCounterSumm="+VfluidCounterSumm +"; VfluidCounter="+VfluidCounter);
                    //Debug.Log("Vfluid="+Vfluid);
                    freeLayerCount.Add(VfluidCounter);
                    VfluidCounterSumm += VfluidCounter;
                    //переходим на верхний слой
                    currentY++;
                }
            }
            //восстанавливаю границу с твердыми
            currentY = currentY_old;
            //
            Debug.Log("freeLayerCount.count=" + freeLayerCount.Count);
            //снизу вверх
            for (int i = 0; i < freeLayerCount.Count; i++)
            {
                Debug.Log("freeLayerCount[" + i + "]=" + freeLayerCount[i]);
            }


            //распределение масс по слоям
            List<double> MassLayerCount = new List<double>();


            //далее, если слоев для размещения жидкости == 1, то распределяем тупо всю массу (все компоненты) по слою
            if (freeLayerCount.Count == 1)
            {
                Debug.Log("1!");
                Debug.Log("m1 = " + summM_fluid);
                MassLayerCount.Add(summM_fluid);
            }

            //если два .... то 
            // или через + m * g(масса вниз)
            // k * (m1 / Rom) / V1 + m1 * g == k * (m2 / Rom) / V2
            // m1 = (m_summ / V2) / (1 / V1 + 1 / V2 + g * Rom / k)
            if (freeLayerCount.Count == 2)
            {
                double V1 = freeLayerCount[1];
                double V2 = freeLayerCount[0];
                double g = 9.81;

                double m1 = (summM_fluid / V2) / (1 / V1 + 1 / V2 + g * avgRo / k); //верхний слой
                double m2 = summM_fluid - m1; //нижний слой

                Debug.Log("2!");
                Debug.Log("m1 = " + m1);
                Debug.Log("m2 = " + m2);

                MassLayerCount.Add(m2);
                MassLayerCount.Add(m1);
            }

            //если три .... то 
            if (freeLayerCount.Count == 3)
            {

                double V1 = freeLayerCount[2]; //нижний слой
                double V2 = freeLayerCount[1];
                double V3 = freeLayerCount[0]; //верхний слой
                /*
                double g = 9.81;

                //double z = avgRo * 9.81 / k;
                double z = 0.01;
                double c1 = 1 / V1 + z + 1 / V2;
                double c2 = 1 / V2 + z + 1 / V3;

                double m2 = summM_fluid / (V3 * (C2 + (1/(V3*V2*(C1 - 1/V2)))));
                double m1 = m2 / (V2* (C1-1/V2));
                double m3 = summM_fluid - m1 - m2;
                */

                double m1 = summM_fluid * (V1 / Vfluid); //нижний слой
                double m2 = summM_fluid * (V2 / Vfluid); //средний
                double m3 = summM_fluid * (V3 / Vfluid); //верхний слой

                double minusM1 = m1 * 0.02;
                m1 -= minusM1;
                m2 += minusM1;
                double minusM2 = m2 * 0.01;
                m2 -= minusM2;
                m3 += minusM2;

                Debug.Log("3!");
                Debug.Log("m1 = " + m1);
                Debug.Log("m2 = " + m2);
                Debug.Log("m3 = " + m3);

                MassLayerCount.Add(m3);
                MassLayerCount.Add(m2);
                MassLayerCount.Add(m1);
            }

            // если ?....то
            if ((freeLayerCount.Count > 3) || (freeLayerCount.Count == 0))
            {
                Debug.Log("Error 14346389265=freeLayerCount.Count= " + freeLayerCount.Count);
            }


            //Ну и собственно распределяем все компоненты жидкости по стоям в моответствии с полученными массами
            for (int i = 0; i < MassLayerCount.Count; i++)
            {
                //i - номер слоя [от нижнего к верхнему]
                //MassLayerCount[i] - сколько массы положить в слой
                //граница с твердыми = currentY;
                //summM_fluid - всего массы для распределения

                //переходим все компоненты в summFluid[] и начинаем с нижнего слоя с наиболее плотных до наиболее легких
                //т.е. сначала распределяем наиболее плотные компоненты , потом легкие
                bool isNext = false;

                while ((summFluid.Count > 0) && (isNext == false))
                {
                    //распределяем MassLayerCount[i] килограмм на слой currentY
                    //summFluid [summFluid.Count-1] - что распределяем []с конца

                    //считаем сколько ячеек есть для распределения в этом слое (не стена) [если все стены - то поднимаемся на уровень выше currentY++]
                    //проходим по всем компонентам, набираем необходимую массу и распределяем ее по установленному количеству ячеек
                    //поднимаемся на уровень выше currentY++
                    //summFluid.RemoveAt(summFluid.Count-1);

                    int countCell = 0;
                    for (int ix = x - _w; ix <= x + _w; ix++)
                    {
                        int _index = ix * maxx + currentY;
                        //если стена
                        bool isWall = false;
                        for (int z = 0; z < map2d[_index].data.components.Count; z++)
                        {
                            if (map2d[_index].data.components[z].type == myComponentType.wall)
                            {
                                isWall = true;
                                break;
                            }
                        }
                        if (isWall == true) continue;
                        countCell++;
                    }
                    //если нет ячеек для распределения на этом слое то идем выше
                    if (countCell == 0)
                    {
                        currentY++;
                        continue;
                    }
                    double massToOneCell;
                    //на каждую свободную ячейку в этом слое нужно распределить .... кг
                    if (MassLayerCount[i] < summFluid[summFluid.Count - 1].m)
                    {
                        massToOneCell = MassLayerCount[i] / (double)countCell;
                    }
                    else
                    {
                        massToOneCell = summFluid[summFluid.Count - 1].m / (double)countCell;
                    }
                    double massDistributed = 0;
                    for (int ix = x - _w; ix <= x + _w; ix++)
                    {
                        int _index = ix * maxx + currentY;
                        //если стена
                        bool isWall = false;
                        for (int z = 0; z < map2d[_index].data.components.Count; z++)
                        {
                            if (map2d[_index].data.components[z].type == myComponentType.wall)
                            {
                                isWall = true;
                                break;
                            }
                        }
                        if (isWall == true) continue;
                        //распределяем massToOneCell из summFluid [summFluid.Count-1]
                        {
                            if (summFluid[summFluid.Count - 1].m > massToOneCell)
                            {
                                //если хватает массы компонента
                                myComponent temp = new myComponent();
                                temp.type = myComponentType.fluid;
                                temp.m = (float)massToOneCell;
                                temp.Ro = summFluid[summFluid.Count - 1].Ro;
                                temp.Q = summFluid[summFluid.Count - 1].Q * ((float)massToOneCell / summFluid[summFluid.Count - 1].m);
                                temp.C = summFluid[summFluid.Count - 1].C;
                                summFluid[summFluid.Count - 1].Q -= temp.Q;
                                summFluid[summFluid.Count - 1].m -= temp.m;
                                if (summFluid[summFluid.Count - 1].m < 0.00001 && ix==x+_w)//Проверка из-за деления на 3
                                {
                                    temp.m += summFluid[summFluid.Count - 1].m;
                                    temp.Q += summFluid[summFluid.Count - 1].Q;
                                    summFluid.RemoveAt(summFluid.Count-1);
                                }
                                map2d[_index].data.components.Add(temp);
                                massDistributed += temp.m;
                                
                                if (massDistributed >= MassLayerCount[i] * 0.99f)
                                {
                                    isNext = true;
                                    currentY++;
                                    break;
                                }
                            }
                            else
                            {
                                //если нехватает, то выбрасываем компонет
                                myComponent temp = new myComponent();
                                temp.type = myComponentType.fluid;
                                temp.m = summFluid[summFluid.Count - 1].m;
                                temp.Ro = summFluid[summFluid.Count - 1].Ro;
                                temp.Q = summFluid[summFluid.Count - 1].Q;
                                temp.C = summFluid[summFluid.Count - 1].C;
                                summFluid.RemoveAt(summFluid.Count - 1);
                                map2d[_index].data.components.Add(temp);
                                massDistributed += temp.m;
                                if (massDistributed >= MassLayerCount[i]*0.99f)
                                {
                                    isNext = true;
                                    currentY++;
                                    break;
                                }
                                break;
                            }
                        }
                    }
                    MassLayerCount[i] -= massDistributed;
                }
            }
            //конец распределения
        }
        //конец распределения жидкости
    }

    private void Step6(int x, int y, int _w, int _h)
    {
        //3. Зная  Vgas, распределяем газ ровно по ячейкам и все, финиш
        //причем все газы в отличие от твердых и жидких распределяем равномерно по одним и темже клеткам
        
        //Ищем сначала доступные объемы в каждом слое и суммарный доступный объем.
        List<double> gasMass = new List<double>();
        //List<float> gasQ = new List<float>();
        if (summGas.Count > 0)
        {
            double summGasMass = 0;
           // float summGasQ = 0;
            for (int i = 0; i < summGas.Count; i++)
            {
                summGasMass += summGas[i].m;
               // summGasQ = summGas[i].Q;
            }
            List<double> availableV = new List<double>();
            double summAvailableV = 0;
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                double thisLayerV = 0;
                for (int ix = x - _w; ix <= x + _w; ix++)
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
                    thisLayerV += map2d[_index].data.V;
                    foreach (var component in map2d[_index].data.components)
                    {
                        // if (component.type == myComponentType.fluid)
                        // {
                        //     thisLayerV-=(Vfluid*component.m/summM_fluid);
                        // }
                        if (component.type == myComponentType.solid)
                        {
                            thisLayerV -= (component.m / component.Ro);
                        }
                    }
                }
                //Вычитаем объем жидкости в данном слое
                int sss = freeLayerCount.Count;
                if (iy == y + _h && freeLayerCount.Count == 3)
                    thisLayerV -= freeLayerCount[2];
                else if (iy == y && freeLayerCount.Count >= 2)
                    thisLayerV -= freeLayerCount[1];
                else if (iy == y - _h && freeLayerCount.Count >= 1)
                    thisLayerV -= freeLayerCount[0];
                availableV.Add(thisLayerV);
                summAvailableV += thisLayerV;
            }
            //Находим доли объема на каждый слой и соответственно считаем массу газа которую на этот слой кинуть надо.
            if(availableV.Count!=0)
                for (int ind = availableV.Count - 1; ind >= 0; ind--)
                {
                    availableV[ind] = availableV[ind] / summAvailableV;
                    gasMass.Add((float)availableV[ind] * summGasMass);
                    //gasQ.Add((float)availableV[ind] * summGasQ);
                }
        }

        for (int index = 0; index < gasMass.Count; index++)
        {
            if(gasMass[index]==0) continue;
            currentY = y - _h + index;
            //Ищем количество доступных ячеек
            int availableCells = 2 * _w + 1;
            for (int ix = x - _w; ix <= x + _w; ix++)
            {
                int _index = ix * maxx + currentY;
                for (int i = 0; i < map2d[_index].data.components.Count; i++)
                {
                    if (map2d[_index].data.components[i].type == myComponentType.wall)
                    {
                        availableCells--;
                        break;
                    }
                }
            }
            if(availableCells==0) continue;
            bool isNext = false;
            while (summGas.Count > 0 && !isNext)
            {
                //заранее запоминаем сколько массы и энергии компонента мы должны сбросить на еденицу объема
                //double massPerVolume = summGas[0].m / Mathf.Ceil((float)Vgas);
                //double QPerVolume = summGas[0].Q / Mathf.Ceil((float)Vgas);
                bool done = false;
                double massToOneCell;
                //на каждую свободную ячейку в этом слое нужно распределить .... кг
                if (gasMass[index] < summGas[summGas.Count - 1].m)
                {
                    massToOneCell = gasMass[index] / availableCells;
                }
                else
                {
                    massToOneCell = summGas[summGas.Count - 1].m / availableCells;
                }
                double QToOneCell = summGas[summGas.Count - 1].Q * massToOneCell / summGas[summGas.Count - 1].m;
                double  massDistributed = 0;
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

                    //итак, есть ячейка, у нее есть объем
                    {
                        myComponent temp = new myComponent();
                        temp.type = myComponentType.gas;
                        // temp.m = (float)massPerVolume * map2d[_index].data.V;
                        temp.m = massToOneCell;
                        temp.Ro = summGas[summGas.Count - 1].Ro;
                        temp.C = summGas[summGas.Count - 1].C;
                        //temp.Q = (float)QPerVolume * map2d[_index].data.V;
                        temp.Q = QToOneCell;
                        summGas[summGas.Count - 1].Q -= temp.Q;
                        summGas[summGas.Count - 1].m -= temp.m;
                        if (summGas[summGas.Count - 1].m < 0.00001 && ix==x+_w)//Проверка из-за деления на 3
                        {
                            temp.m += summGas[summGas.Count - 1].m;
                            temp.Q += summGas[summGas.Count - 1].Q;
                            //summGas.RemoveAt(summGas.Count-1);
                        }
                        map2d[_index].data.components.Add(temp);
                        massDistributed += temp.m;
                        if (massDistributed >= gasMass[index] * 0.99f)
                        {
                            isNext = true;
                            break;
                        }
                    }
                    
                    //если все распределили, удаляем компонент и переходим к следующему
                    if (summGas[summGas.Count - 1].m < 0.0001f)
                    {
                        //done = true;
                        summGas.RemoveAt(summGas.Count-1);
                        break;
                    }

                }
                gasMass[index] -= massDistributed;
                if (gasMass[index] <= 0.0001f)
                {
                    isNext = true;
                }
            }
        }
        //конец распределения газа
    }


    //теплообмен
    private void Step7(int x, int y, int _w, int _h)
    {
        //Q-Джоули, теплота
        //m - кг, масса
        //с - Дж\(кг*Кельвин) - телоемкость

        //1. Находим сумму всех телот (Qsumm)
        //2.Находим сумму summ (m(i) * c(i)), т.е. сумму всех масс помноженных на теплоемкость
        //3. Находим коэффициент распределения k = Qsumm \ summ (m(i) * c(i))
        //4. Зная MC каждого компонента в каждой клетке распределяем телоту в соответствии Q_компонента_ячейка = mc_компонента_ячейки * k

        //например:
        //А) m=2 кг, q=10000 Дж, C= 1000 Дж\(кг*Кельвин)
        //B) m=1 кг, q=5000 Дж , C= 2000 Дж\(кг*Кельвин)
        //C) m=3 кг, q=1000 Дж , C= 3000 Дж\(кг*Кельвин)

        //SummQ = Q1+Q2+Q3 = 16000 Дж
        //summ(mc) = 2*1000 + 1*2000 + 3*3000 = 13000 Дж\Кельвин
        //k = SummQ \ summ(mc) = 1.233
        //Q1 = m1c1*1.233 = 2460 Дж
        //Q2 = m2C2*1.233 = 2460 Дж
        //Q3 = m3c3*1.233= 11000 Дж
        //проверка Q1+Q2+Q3 = 16000. Все ок.

        double summQ = 0;
        double summMC = 0;

        for (int iy = y + _h; iy >= y - _h; iy--)
        {
            for (int ix = x - _w; ix <= x + _w; ix++)
            {
                int _index = ix * maxx + iy;
                foreach (var component in map2d[_index].data.components)
                {
                    summQ += component.Q;
                    summMC += component.m * component.C; 
                }
            }
        }

        if (summMC == 0) return;

        double k = summQ / summMC;

        Debug.Log("summQ=" + summQ);
        Debug.Log("summMC=" + summMC);
        Debug.Log("k=" + summQ);

        for (int iy = y + _h; iy >= y - _h; iy--)
        {
            for (int ix = x - _w; ix <= x + _w; ix++)
            {
                int _index = ix * maxx + iy;
                foreach (var component in map2d[_index].data.components)
                {
                    double MC = component.m * component.C;
                    component.Q = (float) (MC * k);
                }
            }
        }

    }


    private void Shot(int x, int y)
    {
        //расчет свертки 3*3

        //идея:
        //зная векторную картину перемещений, можем оптимизировать порядок выборки координат для просчетов методом монтекарло
        //т.е. будем стараться чаще и вперед просчитывать области, активно отдающие массу для ее скорейшего распределения

        //координаты центра свертки
        //const int x = 27; //27
        //const int y = 44; //44

        //свертка 3*3  ! только нечетные размеры
        const int width = 3;
        const int height = 3;
        int _w = (width - 1) / 2;
        int _h = (height - 1) / 2;
        //

        Step1(x, y, _w, _h);    //суммирую вообще все 3*3 в 3 списка (твердое, жидкое, газ) вместе со всеми их энергиями и т.д.
        Step2();                //проходим все 3 списка и сливаем "подобные с подобными" (сравнивая плотности) и переспределяя количество теплоты
        Step3();                //вычисляем объемы с учетом равенства силы (давления) между газом/жидкостью в фиксированном объеме
        Step4(x, y, _w, _h);    //заполняю сначала твердыми снизу ровно в один уровень
        Step5(x, y, _w, _h);    //заполняю жидкими 
        Step6(x, y, _w, _h);    //заполняю газами
        Step7(x, y, _w, _h);    //теплообмен

        /*
         * 
            Массобмен сделан
            Завтра теплообмен
            Потом уже межфазные переходы
            Потом эммитторы и коллекторы
            Потом датчики и машалки и подогреватели ......, адсорберы и абсорберы всяческие
            Потом тест
            Потом склейка с редактором потоков
            Потом тест
            Потом создание библиотеки аппаратов и финал
         */



        /* отрисовка
        for (int ix = x - _w; ix <= x + _w; ix++)
        {
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                int _index = ix * maxx + iy;
                map2d[_index].ManualUpdate();
            }
        }
        */
        //Конец OneStep
    }

    public void OneStep()
    {
        //Shot(27, 44);

        // Shot(26, 44);
        // ShowAllMass(26,41,43,48);

        System.DateTime datevalue1 = System.DateTime.Now;

        

        if (true)
            for (int i = 0; i < 250; i++)
            {
                //int rx = (int)Random.Range(27f, 41f);
                //int ry = (int)Random.Range(44f, 48f);
                int rx = Random.Range(27, 41);
                int ry=Random.Range(44, 48);
                //Debug.LogWarning("Сгенерированный RX=" + rx + " ;Сгенерированный RY=" + ry);
                Shot(rx, ry);
            }

        System.DateTime datevalue2 = System.DateTime.Now;
        double diff = (datevalue2 - datevalue1).TotalMilliseconds;

        Debug.Log("calc mm millisec = " + diff);
        Debug.Log("calc mm millisec / 250 = " + (int)diff/250);

        // ShowAllMass(26,41,43,48);
        for (int t = 0; t < maxx * maxy; t++)
        {
            map2d[t].ManualUpdate();
        }

        diff = (System.DateTime.Now - datevalue2).TotalMilliseconds;

        Debug.Log("repaint millisec = " + diff);
    }

    // private void ShowAllMass(int xMin,int xMax, int yMin,int yMax)
    // {
    //     float massFluid=0f, massGas=0f, massSolid=0f;
    //     for (int i = xMin; i < xMax; i++)
    //     {
    //         for (int j = yMin; j < yMax; j++)
    //         {
    //             int _index = i * maxx + j;
    //             //Debug.Log("ix=" + ix + " " + "iy=" + iy);
    //             //Debug.Log("index1=" + _index + " " + "index2="  + map2d[_index].index);
    //             //суммирую всю твердое и кидаю вниз
    //             bool isWall = false;
    //             if (map2d[_index].data.components.Count > 0)
    //             {
    //                 //прохожу все компоненты в этой клетке
    //                 foreach (var component in map2d[_index].data.components)
    //                 {
    //                     if (component.type == myComponentType.gas)
    //                     {
    //                         massGas += component.m;
    //                     }
    //                     else if (component.type == myComponentType.fluid)
    //                     {
    //                         massFluid += component.m;
    //                     }
    //                     else if (component.type == myComponentType.solid)
    //                     {
    //                         massSolid += component.m;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     Debug.LogError("MassFluid="+massFluid);
    //     Debug.LogError("MassSolid="+massSolid);
    //     Debug.LogError("MassGas="+massGas);
    // }
}
