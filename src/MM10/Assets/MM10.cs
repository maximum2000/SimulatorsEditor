/***************************************************************************
MM10.cs -  ������ �������������� ������ ���������
-------------------
begin                : 24 ��� 2023
copyright            : (C) 2023 by ������ ������ ���������� (maximum2000)
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
        //����:
        //���� ����� ������ ���������� ������� ������ �������� ������������, ����� �������... ��� ����� ����� ���������
        //���� ����� ���������� �����, ��� �����.. ������������ ���� � ������� ... ����� ��������� �������� � ����������� ����
        //���� ����� ������ � ������� ����������� ... � �� ������������ � �������... ����� ��������� � �� �����������
        //����� ����������� ������������ � ������������� ������� ��� ����������� ��������� (����������� ���� ����������� �������������)

        //����:
        //���� ��������� ������� �����������, ����� �������������� ������� ������� ��������� ��� ��������� ������� ����������
        //�.�. ����� ��������� ���� � ������ ������������ �������, ������� �������� ����� ��� �� ���������� �������������

        //!!!

        //0. ������ ��� ����� ������������, � ������
        //1. ���� ���� ����� ����� �� ������� ���� �������
        //   (������� ��� � ������� ���������� ��������� � ��������)
        //2. ���� ���� ����� ����� �� ������� ���� ��������
        //3. ��������� ������ �����\�����\������ ��� �������� � �������, �.�. ���������������� �������������
        //4. ������� ��������� ������������ � ���� ��� ����, �� ������������ ��� ����� �� ���������� ������

        //���������� ������ �������
        const int x = 27; //27
        const int y = 44; //44

        //������� 3*3  ! ������ �������� �������
        const int width = 3;
        const int height = 3;
        int _w = (width - 1) / 2;
        int _h = (height - 1) / 2;
        //

        //�������� ������ ��� 3*3 � 3 ������ (�������, ������, ���) ������ �� ����� �� ��������� � �.�.
        List<myComponent> summSolid = new List<myComponent>();
        List<myComponent> summFluid = new List<myComponent>();
        List<myComponent> summGas = new List<myComponent>();
        //������� ������
        double summV = 0;
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




        //��������� ����� � ������ ��������� ���� (��������) ����� �����/��������� � ������������� ������
        //summV - ������ �����
        double Vsolid = 0;
        double C1 = 0;
        double C2 = 0;
        double Vfluid = 0;
        double Vgas = 0;

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

            //����������� ������ �������� �����
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





        //� ������ �������� 3*3 ������� �������� ����� ����� � ���� �������, ����� ������� ���� ������� � ���� �������, ����� ���������� �����
        //�� ������� ��� �������, ������������ � �.�.
        {
            //1. ���� Vsolid � �����/��������� ������� - ������� ������� ������ �� ��������, ������� ������ �����, ������� ��������, ���������
            //���������� ��������� ������� ������, � ��� ��� ������ �� ��������, ������ �������� ��������, ������� ����� ������� ����� ����� ������

            //���� ���� ���������������� �������.....
            int currentY = y - _h;

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
                    for (int i = 0; i<  map2d[_index].data.components.Count; i++) 
                    {
                        freeLayerV -= map2d[_index].data.components[i].m / map2d[_index].data.components[i].Ro;
                    }
                }

                Debug.Log("Vcomponent=" + Vcomponent);
                Debug.Log("freeLayerV=" + freeLayerV);
                Debug.Log("availableCells=" + availableCells);

                //���, ������� ��� ��� �� ���� � ����������� ��������� � ����������� ��������� ����� 

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
            //����� ������������� �������


            //2. ���� Vfluid � ����� �������� � ��������� ��������� - ������� ������� ������ �� ��������, ������� ������ �����, ������� ��������, ���������
            //���������� ��������� ������� ������, � ��� ��� ������ �� ��������
            //������ �������� ��������, ������� ����� ��������� ����� ������, � ������������ � ����������, �.�. ������� ����, ����� �����
            //� ��� �� �������� ��� ��������, ������� ������������� �� RoGH �� ������� � ������ ����� / ����������� �� RoGH �� ������ � �������

            //�������� ������� ��....1.������� �������� �� �������� ����, � �.�.� ������ �������� ���� ������, �.�.�����������
            //������ ����� ��������� ������ ����� ��� ������ �� ������ ���� ��������
            //��� ������������� �������� ����� ������� ��� ����� ���� ������� � ���� ������������ ����� ������ ���

            //���� ���� ���������������� ��������.....
            //currentY - �� ����� ���� ������������ �� ������������� �������

            //������������ ������ � ��� (�� ������� ���-��������), �.�. � ��� ����� �� ����� �������� == �������� ����
            //���� ���� ���, �� ������������� �� �������� �������� ��� ������ �����
            //Vgas - ��������, ����� ��� ������� �����
            //Vsolid - ��������, ����� ��� ������� ��������
            //Vfluid - ��������, ����� �������� ������� ���������� ������������
            //Fgas == Fgas - ���� (��������) �� ������� ������� "���/��������"

            /*
             * ��������:
             * 1. ���� �� ������� ��������������� "��������/���" (���� ���� ���, �� � ������ �����)
             * 2. ���� ����� ������� �������� ����� � ����� ������ - ����� ����, � �������� ������ ����������
             * 3. ���� �� ���� ����������� �������� �� ����� ������� � ����� ������� �...
             *    �������� ����� ������� �����, ����� ������������ � ������� ����, � ������ ��������� �������� � ����� (��� ������� ����) ��� (���� ���� ���) �� � "Ffluid"
             *    ���� ���� ��������, � ����� ���������� ��� ��������, ��������� � ������� ���� � ����������� �������� �� [mg] ��� [qgh], ���� qgh �� �������� �� ������ ��������� q1*g*h + q2*g*h + ....
            */

            /*
             * ��������:
             * 1. ���� �� ������� ��������������� "��������/���" (���� ���� ���, �� � ������ �����)
             * 2. ���� ����� ������� �������� ����� � ����� ������ - ����� ����, � �������� ������ ����������
             *    ������� ���������� �����, ������� ���������� ���������
             *    ������� ������� ��������� �������� (Rom)
             *    ������� ������������ ������������� ����� �� ����� ����� ����������� ������� 
             *    F�0 = F����
             *    F�1 = k * Summ (m/Rom) / V� + Rom*g*h
             *    F�2 = k * Summ (m/Rom) / V�
             *    ..
             *    ��� �������� ������� ����
             *    k * Summ (m/Rom) / V�  =  F�0
             *    F�1 == F�2 - ��� �����������
             * 
             * 
             * 3. ���� �� ���� ����������� �������� �� ����� ������� � ����� ������� �...
             *    �������� ����� ������� �����, ����� ������������ � ������� ����, � ������ ��������� �������� � ����� (��� ������� ����) ��� (���� ���� ���) �� � "Ffluid"
             *    ���� ���� ��������, � ����� ���������� ��� ��������, ��������� � ������� ���� � ����������� �������� �� [mg] ��� [qgh], ���� qgh �� �������� �� ������ ��������� q1*g*h + q2*g*h + ....
            */



            while (summFluid.Count > 0)
            {
                summFluid.RemoveAt(0);
            }
            //����� ������������� ��������


            //3. ����  Vgas, ������������ ��� ����� �� ������� � ���, �����
            //������ ��� ���� � ������� �� ������� � ������ ������������ ���������� �� ����� � ����� �������

            //���� ���� ���������������� ����.....
            //currentY = y + _h;
            while (summGas.Count > 0)
            {
                //������� ���������� ������� ����� � ������� ���������� �� ������ �������� �� ������� ������
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
            //����� ������������� ����

        }




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
            temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
            temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
            temp.Q = temp.C * temp.m * 273.15f;  //��
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.fluid)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.fluid;
            temp.m = 1f;
            temp.Ro = 1000f; //��\�3
            temp.C = 4180.6f; //�������� ����������� ����, ��/(��K)
            temp.Q = temp.C * temp.m * 273.15f;  //��
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.solid)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.solid;
            temp.m = 1f;
            temp.Ro = 1700; //��\�3
            temp.C = 835f; //�������� ����������� ����, ��/(��K)
            temp.Q = temp.C * temp.m * 273.15f;  //��
            cell.data.components.Add(temp);
            cell.ManualUpdate();
        }
        else if (selectedType == ModeType.wall)
        {
            cell.data.Clear();
            myComponent temp = new myComponent();
            temp.type = myComponentType.wall;
            temp.m = 1f;
            temp.Ro = 7800f; //��\�3
            temp.C = 460f; //�������� ����������� ����, ��/(��K)
            temp.Q = temp.C * temp.m * 273.15f;  //��
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


        //������
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


        //�������
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
