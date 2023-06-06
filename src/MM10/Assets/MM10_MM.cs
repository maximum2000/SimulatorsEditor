/***************************************************************************
MM10_MM.cs -  ������ �������������� ������ ���������
-------------------
begin                : 24 ��� 2023
copyright            : (C) 2023 by ������ ������ ���������� (maximum2000)
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
    //������� ������
    private double summV = 0;
    private double Vsolid = 0;
    private double Vfluid = 0;
    private double Vgas = 0;
    //����������� ������ �������� �����
    private double k = 1000000.0;
    private int currentY = 0;


    private void Step1(int x, int y, int _w, int _h)
    {
        //�������� ������ ��� 3*3 � 3 ������ (�������, ������, ���) ������ �� ����� �� ��������� � �.�.
        summSolid = new List<myComponent>();
        summFluid = new List<myComponent>();
        summGas = new List<myComponent>();
        //������� ������
        summV = 0;
        //
        for (int ix = x - _w; ix <= x + _w; ix++)
        {
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                int _index = ix * maxx + iy;
                //Debug.Log("ix=" + ix + " " + "iy=" + iy);
                //Debug.Log("index1=" + _index + " " + "index2="  + map2d[_index].index);
                //�������� ��� ������� � ����� ����
                bool isWall = false;
                if (map2d[_index].data.components.Count > 0)
                {
                    //������� ��� ���������� � ���� ������
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
                //��������� ��������� ����� ������
                if (isWall == false)
                {
                    summV += map2d[_index].data.V;
                }
                //������ ������ �� ������
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
        //����� ������������
    }

    private void Step2()
    {
        //�������� ��� 3 ������ � ������� "�������� � ���������" (��������� ���������) � ������������� ���������� �������
        //�.�. �� 3� ������� ��� ����� ������ ��������� ���� ��������������
        ////������ ���� ��� �������� ����� ���� ��������� ������ �������� ���������� � "������" ��� ����� ���������� � "1" � ������ ����������������� �������
        //�.�. ������ ��������� ������ ��������� ��������
        //����� ������ ������� � ���� ���������� �� ���������� � �������, ���� ������� � ������, �� ������� �������
        {
            if (summSolid.Count > 0)
            {
                for (int f = 0; f < summSolid.Count; f++)
                {
                    for (int b = summSolid.Count - 1; b > f; b--)
                    {
                        //!!!! �������� ��������� � ���������
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
                        //!!!! �������� ��������� � ���������
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
                        //!!!! �������� ��������� � ���������
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
        //����� ������


        //��������� �� ��������� ��� ������
        {

        }
    }


    private void Step3()
    {
        //��������� ����� � ������ ��������� ���� (��������) ����� �����/��������� � ������������� ������
        //summV - ������ �����
         Vsolid = 0;
        double C1 = 0;
        double C2 = 0;
         Vfluid = 0;
         Vgas = 0;

        double Ffluid = 0;
        double Fgas = 0;

        

        {
            // Vsolid = Summ (m/Ro)  - ����� ������� �� ������� �� ���� � ����
            // F�=F� �� ������� �������,
            // F� = Summ  (m*E) / V�
            // F� = k * Summ (m/Ro) / V�
            // F� = �1 / V�
            // F� = �2 / V�
            // C1/V� = C2/V�
            // V� = �1/c2 * V�
            // V� = (V����� - Vsoild) / (�1/�2+1) 
            // V� = V����� - Vsoild - V�



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
        //� ������ �������� 3*3 ������� �������� ����� ����� � ���� �������, ����� ������� ���� ������� � ���� �������, ����� ���������� �����
        //�� ������� ��� �������, ������������ � �.�.
        {
            //1. ���� Vsolid � �����/��������� ������� - ������� ������� ������ �� ��������, ������� ������ �����, ������� ��������, ���������
            //���������� ��������� ������� ������, � ��� ��� ������ �� ��������, ������ �������� ��������, ������� ����� ������� ����� ����� ������

            //���� ���� ���������������� �������.....
            currentY = y - _h;

            while (summSolid.Count > 0)
            {
                //��� ����� ����� ������... 
                //��� ������� ���� ������ ������� ����� � ���� ��������....

                //������ ������� ���������� ����� � ���� ����.... �����
                //�) ���� ��������� ������� ����� ��� ������������� �������� ���������� � ��� ��������, �� ����������� �� ����
                //   � ������ ���������
                //   currentY �� �����
                //�) ���� ��������� ������� ����� ��� ������������� �������� ���������� � ����� � ���� �����������,
                //   �� ����������� �� ���� � ������ ���������,
                //   �������� ���� ���� - �����  currentY (currentY++ ���� ��� ���� ���������������� ����������...
                //   ...���� ��� �����  - ��� �� ������ (��� ������ ������)
                //�) ���� ��������� ������� ���� ���� ������� ����������� � ��� ��������� ��������
                //   ,�� ����������� ���� ���������, ������� ����� � ������� �� ���������� � 
                //   �������� ���� ���� - �����  currentY (currentY++ ���� ��� ���� ���������������� ����������...
                //  ...���� ��� �����  - ��� �� ������ (��� ������ ������)
                //�) ���� ����� � ���� ������ ��� - ��������� ����
                //   �������� ���� ���� - �����  currentY (currentY++ ���� ��� ���� ���������������� ����������...
                //  ...���� ��� �����  - ��� �� ������ (��� ������ ������)

                //����� ��������� ��� ������������� �������� ����������
                double Vcomponent = summSolid[0].m / summSolid[0].Ro;

                //��������� ����� � ���� ����=
                double freeLayerV = 0;
                //����� �������� � ���� ����
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

                //���, ������� ��� ��� �� ���� � ����������� ��������� � ����������� ��������� ����� 

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
            //����� ������������� �������
        }
    }


    private void Step5(int x, int y, int _w, int _h)
    {
        //2. ��������� ��������, ���� Vfluid � ����� ��������� � ��������� ���������
        //������ �������� ��������, ������� ����� ��������� ����� ������, � ������������ � ����������, �.�. ������� ����, ����� �����
        //� ��� �� �������� ��� ��������, ������� ������������� �� RoGH �� ������� � ������ ����� / ����������� �� RoGH �� ������ � �������

        //����:
        //������������ ������ � ��� (�� ������� ���-��������), �.�. � ��� ����� �� ����� �������� == �������� ����
        //���� ���� ���, �� ������������� �� �������� �������� ��� ������ �����
        //Vgas - ��������, ����� ��� ������� �����
        //Vsolid - ��������, ����� ��� ������� ��������
        //Vfluid - ��������, ����� �������� ������� ���������� ������������
        //Fgas == Fgas - ���� (��������) �� ������� ������� "���/��������"
        //currentY - �� ����� ���� ������������ �� ������������� �������

        /*
         * ��������:
         *  ! ��������� ����� �� ������� ����������� �������� (�2/V) , �� ����� ��  F���� == F�������_���� == ... == F����������_���� - �� �� ������, ����� �� ����� ����� � �������
         *  ! � ���� �� ����� ��������� �� ���� �� � ��� ����� �� ������ �� ��������� ����, �.�. �������� ����� ������ ��� �������, ���� ��������� ����� �� ���������� ������ ������� �������� ��� �����
         *  ! ������� �� �������������� � ������ ������������ ����� �������� �� ������ �� �������� ���� �� ������ ����....
         *  
         * 1. ���� �� ������� ��������������� "��������/���" (���� ���� ���, �� � ������ �����)
         * 2. ���� ����� ������� �������� ����� � ����� ������ - ����� ����, � �������� ������ ����������
         * 3. ������� ���������� �����, ������� ���������� ���������
         * 4. ������� ������� ��������� �������� (Rom)
         * 5. ������� ������������ ������������� ����� �� ����� ����� ����������� ������� , ��, ���������� �� �������� ����
         *    F�1 = k * Summ (m/Rom) / V� + Rom*g*h  [��� �������� ����]
         *    F�2 = k * Summ (m/Rom) / V�            [���� ������� ����]
         *    F�1 == F�2 
         *              
         * 6. ���� �� ���� ����������� �������� �� ����� ������� � ����� ������� �...
         *    �������� ����� ������� �����, ������� ����� ����� �� ���� 5
         *    
         *    
         *    ��� �����.... ������� ������ ����� ����� ������ ���������, ����� 3 ��������
         *    1 ����. ��� �������, ������������ ��� ���������� �������� � ���� ����, ���
         *    2 ����. ������� ������� ��������� ����� ��� �������� � ������� ����, ������� � ������, ���������� ��������� F�1 == F�2  � ������������
         *    3 ����. ������� ������� ���������� ������ � �������, � ������� � � ������... ��������� ��������� F�1 == F�2 == F�3 � ������������
         *
         *    ��� ������ 2
         *    ��� ����� + Rom*g*h1    (����� ��������)
         *    k * m1 / Rom / V1 + Rom*g*h1    == k * m2 / Rom / V2 
         *    ��� ����� + m*g         (����� ����)
         *    k * (m1/Rom)/V1 + m1*g   == k* (m2/Rom)/V2
         *    m1 = (m_summ/V2) / (1/V1 + 1/V2 + g*Rom/k)
         *    
         *    ��� ������ 3
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

        if (summFluid.Count > 0)
        {
            //����������� ������� ��������� (��� ����� ����� �� ����� �����)
            double summM_fluid = 0;
            for (int i = 0; i < summFluid.Count; i++)
            {
                summM_fluid += (double)summFluid[i].m;
            }
            Debug.Log("summM_fluid=" + summM_fluid);
            Debug.Log("Vfluid=" + Vfluid);
            double avgRo = summM_fluid / Vfluid;
            Debug.Log("avgRo=" + avgRo);

            //������ ���������� �����
            //������� � ���� ��� ����������� ������� ���������� � ��� ������ ������ ��������� �����, ���� �� ����������....
            //��������� ��������� ����� � ������ ����
            //���������� ��������� � ������� - ���������� �����

            //������������� ������� �� �����
            List<double> freeLayerCount = new List<double>();
            //currentY - �� ����� ���� ������������ �� ������������� �������
            bool isStop = false;
            double VfluidCounterSumm = 0;
            int currentY_old = currentY;
            while (isStop == false)
            {
                double VfluidCounter = 0;
                for (int ix = x - _w; ix <= x + _w; ix++)
                {
                    int _index = ix * maxx + currentY;

                    //���� �����
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

                    //������� ��������� ���������  �����
                    VfluidCounter += map2d[_index].data.V;
                    for (int i = 0; i < map2d[_index].data.components.Count; i++)
                    {
                        if (map2d[_index].data.components[i].type == myComponentType.solid)
                        {
                            VfluidCounter -= map2d[_index].data.components[i].m / map2d[_index].data.components[i].Ro;
                        }
                    }
                }

                if (VfluidCounterSumm + VfluidCounter >= Vfluid)
                {
                    isStop = true;
                    freeLayerCount.Add(Vfluid - VfluidCounterSumm);
                    break;
                }
                else
                {
                    freeLayerCount.Add(VfluidCounter);
                    VfluidCounterSumm += VfluidCounter;
                    //��������� �� ������� ����
                    currentY++;
                }
            }
            //�������������� ������� � ��������
            currentY = currentY_old;
            //
            Debug.Log("freeLayerCount.count=" + freeLayerCount.Count);
            //����� �����
            for (int i = 0; i < freeLayerCount.Count; i++)
            {
                Debug.Log("freeLayerCount[" + i + "]=" + freeLayerCount[i]);
            }


            //������������� ���� �� �����
            List<double> MassLayerCount = new List<double>();


            //�����, ���� ����� ��� ���������� �������� == 1, �� ������������ ���� ��� ����� (��� ����������) �� ����
            if (freeLayerCount.Count == 1)
            {
                Debug.Log("1!");
                Debug.Log("m1 = " + summM_fluid);
                MassLayerCount.Add(summM_fluid);
            }

            //���� ��� .... �� 
            // ��� ����� + m * g(����� ����)
            // k * (m1 / Rom) / V1 + m1 * g == k * (m2 / Rom) / V2
            // m1 = (m_summ / V2) / (1 / V1 + 1 / V2 + g * Rom / k)
            if (freeLayerCount.Count == 2)
            {
                double V1 = freeLayerCount[1];
                double V2 = freeLayerCount[0];
                double g = 9.81;

                double m1 = (summM_fluid / V2) / (1 / V1 + 1 / V2 + g * avgRo / k); //������� ����
                double m2 = summM_fluid - m1; //������ ����

                Debug.Log("2!");
                Debug.Log("m1 = " + m1);
                Debug.Log("m2 = " + m2);

                MassLayerCount.Add(m2);
                MassLayerCount.Add(m1);
            }

            //���� ��� .... �� 
            if (freeLayerCount.Count == 3)
            {

                double V1 = freeLayerCount[2]; //������ ����
                double V2 = freeLayerCount[1];
                double V3 = freeLayerCount[0]; //������� ����
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

                double m1 = summM_fluid * (V1 / Vfluid); //������ ����
                double m2 = summM_fluid * (V2 / Vfluid); //�������
                double m3 = summM_fluid * (V3 / Vfluid); //������� ����

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

            // ���� ?....��
            if ((freeLayerCount.Count > 3) || (freeLayerCount.Count == 0))
            {
                Debug.Log("Error 14346389265=freeLayerCount.Count= " + freeLayerCount.Count);
            }


            //�� � ���������� ������������ ��� ���������� �������� �� ����� � ������������ � ����������� �������
            for (int i = 0; i < MassLayerCount.Count; i++)
            {
                //i - ����� ���� [�� ������� � ��������]
                //MassLayerCount[i] - ������� ����� �������� � ����
                //������� � �������� = currentY;
                //summM_fluid - ����� ����� ��� �������������

                //��������� ��� ���������� � summFluid[] � �������� � ������� ���� � �������� ������� �� �������� ������
                //�.�. ������� ������������ �������� ������� ���������� , ����� ������
                bool isNext = false;

                while ((summFluid.Count > 0) && (isNext == false))
                {
                    //������������ MassLayerCount[i] ��������� �� ���� currentY
                    //summFluid [summFluid.Count-1] - ��� ������������ []� �����

                    //������� ������� ����� ���� ��� ������������� � ���� ���� (�� �����) [���� ��� ����� - �� ����������� �� ������� ���� currentY++]
                    //�������� �� ���� �����������, �������� ����������� ����� � ������������ �� �� �������������� ���������� �����
                    //����������� �� ������� ���� currentY++
                    //summFluid.RemoveAt(summFluid.Count-1);

                    int countCell = 0;
                    for (int ix = x - _w; ix <= x + _w; ix++)
                    {
                        int _index = ix * maxx + currentY;
                        //���� �����
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

                    //���� ��� ����� ��� ������������� �� ���� ���� �� ���� ����
                    if (countCell == 0)
                    {
                        currentY++;
                        continue;
                    }

                    //�� ������ ��������� ������ � ���� ���� ����� ������������ .... ��
                    double massToOneCell = MassLayerCount[i] / (double)countCell;
                    double massDistributed = 0;
                    for (int ix = x - _w; ix <= x + _w; ix++)
                    {
                        int _index = ix * maxx + currentY;
                        //���� �����
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
                        //������������ massToOneCell �� summFluid [summFluid.Count-1]
                        {
                            if (summFluid[summFluid.Count - 1].m > massToOneCell)
                            {
                                //���� ������� ����� ����������
                                myComponent temp = new myComponent();
                                temp.type = myComponentType.fluid;
                                temp.m = (float)massToOneCell;
                                temp.Ro = summFluid[summFluid.Count - 1].Ro;
                                temp.Q = summFluid[summFluid.Count - 1].Q * ((float)massToOneCell / summFluid[summFluid.Count - 1].m);
                                temp.C = summFluid[summFluid.Count - 1].C;
                                summFluid[summFluid.Count - 1].Q -= temp.Q;
                                summFluid[summFluid.Count - 1].m -= temp.m;
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
                                //���� ���������, �� ����������� ��������
                                myComponent temp = new myComponent();
                                temp.type = myComponentType.fluid;
                                temp.m = summFluid[summFluid.Count - 1].m;
                                temp.Ro = summFluid[summFluid.Count - 1].Ro;
                                temp.Q = summFluid[summFluid.Count - 1].Q;
                                temp.C = summFluid[summFluid.Count - 1].C;
                                summFluid.RemoveAt(summFluid.Count - 1);
                                map2d[_index].data.components.Add(temp);
                                massDistributed += temp.m;
                                if (massDistributed >= MassLayerCount[i])
                                {
                                    isNext = true;
                                    currentY++;
                                    break;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            //����� �������������
        }
        //����� ������������� ��������
    }

    private void Step6(int x, int y, int _w, int _h)
    {
        //3. ����  Vgas, ������������ ��� ����� �� ������� � ���, �����
        //������ ��� ���� � ������� �� ������� � ������ ������������ ���������� �� ����� � ����� �������

        //���� ���� ���������������� ����.....
        //currentY = y + _h;
        while (summGas.Count > 0)
        {
            //������� ���������� ������� ����� � ������� ���������� �� ������ �������� �� ������� ������
            double massPerVolume = summGas[0].m / Mathf.Ceil((float)Vgas);
            double QPerVolume = summGas[0].Q / Mathf.Ceil((float)Vgas);
            bool done = false;
            //

            for (int iy = y + _h; iy >= y - _h; iy--)
            {
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

                    //����, ���� ������, � ��� ���� �����, ���� �� ���������� massPerVolume * �� ����� ������ 
                    {
                        myComponent temp = new myComponent();
                        temp.type = myComponentType.gas;
                        temp.m = (float)massPerVolume * map2d[_index].data.V;
                        temp.Ro = summGas[0].Ro;
                        temp.C = summGas[0].C;
                        temp.Q = (float)QPerVolume * map2d[_index].data.V;
                        map2d[_index].data.components.Add(temp);
                    }

                    //������� �������������� ����� �����
                    summGas[0].m -= (float)massPerVolume * map2d[_index].data.V;
                    //���� ��� ������������, ������� ��������� � ��������� � ����������
                    if (summGas[0].m < massPerVolume / 10f)
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
        //����� ������������� ����
    }

    public void OneStep()
    {
        //������ ������� 3*3

        //����:
        //���� ��������� ������� �����������, ����� �������������� ������� ������� ��������� ��� ��������� ������� ����������
        //�.�. ����� ��������� ���� � ������ ������������ �������, ������� �������� ����� ��� �� ���������� �������������

        //���������� ������ �������
        const int x = 27; //27
        const int y = 44; //44

        //������� 3*3  ! ������ �������� �������
        const int width = 3;
        const int height = 3;
        int _w = (width - 1) / 2;
        int _h = (height - 1) / 2;
        //

        Step1(x,y, _w, _h);     //�������� ������ ��� 3*3 � 3 ������ (�������, ������, ���) ������ �� ����� �� ��������� � �.�.
        Step2();                //�������� ��� 3 ������ � ������� "�������� � ���������" (��������� ���������) � ������������� ���������� �������
        Step3();                //��������� ������ � ������ ��������� ���� (��������) ����� �����/��������� � ������������� ������
        Step4(x, y, _w, _h);    //�������� ������� �������� ����� ����� � ���� �������
        Step5(x, y, _w, _h);    //�������� ������� 
        Step6(x, y, _w, _h);    //�������� ������


        //���������
        for (int ix = x - _w; ix <= x + _w; ix++)
        {
            for (int iy = y + _h; iy >= y - _h; iy--)
            {
                int _index = ix * maxx + iy;
                map2d[_index].ManualUpdate();
            }
        }

        //����� OneStep
    }

}
