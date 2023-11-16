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

public partial class MM10 : MonoBehaviour
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
        edit,
        methane, // 7
        air,
        water,
        oil,
        sand,
        paraffin
    }

    public ModeType selectedType;

    public void CellClick(ref MyElementUI cell)
    {
        //if (selectedType == ModeType.none)
        //{
        //    //cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 0f, 0);
        //}
        //else if (selectedType == ModeType.gas)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.fluid)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.fluid;
        //    temp.m = 1f;
        //    temp.Ro = 1000f; //��\�3
        //    temp.C = 4180.6f; //�������� ����������� ����, ��/(��K)
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.solid)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.solid;
        //    temp.m = 1f;
        //    temp.Ro = 1700; //��\�3
        //    temp.C = 835f; //�������� ����������� ����, ��/(��K)
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.wall)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.wall;
        //    temp.m = 1f;
        //    temp.Ro = 7800f; //��\�3
        //    temp.C = 460f; //�������� ����������� ����, ��/(��K)
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.delete)
        //{
        //    cell.data.Clear();
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.edit)
        //{
        //    editedCellIndex = cell.index;
        //    CellEditor.SetActive(true);
        //    EditorShowComponentValues(0);
        //}
        //else if (selectedType == ModeType.methane)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.air)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.water)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.oil)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.sand)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        //else if (selectedType == ModeType.paraffin)
        //{
        //    cell.data.Clear();
        //    myComponent temp = new myComponent();
        //    temp.type = myComponentType.gas;
        //    temp.m = 1f;
        //    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
        //    temp.C = 1010f; //� ��������� �� -50 �� 120�� �� �������� ����������� �� �������� � � ���� �������� ������� ������������ ������� ����� 1010 ��/(������).
        //    temp.Q = temp.C * temp.m * 273.15f;  //��
        //    cell.data.components.Add(temp);
        //    cell.ManualUpdate();
        //}
        switch (selectedType)
        {
            case ModeType.none:
                {
                    //cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 0f, 0);
                }
                break;
            case ModeType.gas:
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
                break;
            case ModeType.fluid:
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
                break;
            case ModeType.solid:
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
                break;
            case ModeType.wall:
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
                break;
            case ModeType.delete:
                {
                    cell.data.Clear();
                    cell.ManualUpdate();
                }
                break;
            case ModeType.edit:
                {
                    editedCellIndex = cell.index;
                    CellEditor.SetActive(true);
                    EditorShowComponentValues(0);
                }
                break;
            case ModeType.methane:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.gas;
                    temp.m = 1f;
                    temp.Ro = 0.7168f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 2165f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
            case ModeType.air:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.gas;
                    temp.m = 1f;
                    temp.Ro = 1.2754f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 1010f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
            case ModeType.water:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.fluid;
                    temp.m = 1f;
                    temp.Ro = 1000f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 4180.6f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
            case ModeType.oil:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.fluid;
                    temp.m = 1f;
                    temp.Ro = 910f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 1670f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
            case ModeType.sand:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.solid;
                    temp.m = 1f;
                    temp.Ro = 1700f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 835f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
            case ModeType.paraffin:
                {
                    cell.data.Clear();
                    myComponent temp = new myComponent();
                    temp.type = myComponentType.solid;
                    temp.m = 1f;
                    temp.Ro = 900f; //����������� 0 ��, �������� 100 ���, ������� ���������
                    temp.C = 2190f;
                    temp.Q = temp.C * temp.m * 273.15f;  //��
                    cell.data.components.Add(temp);
                    cell.ManualUpdate();
                }
                break;
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
            map2d[editedCellIndex].data.components[z].m = double.Parse(CellEditorM.text);
            map2d[editedCellIndex].data.components[z].Ro = double.Parse(CellEditorRo.text);
            map2d[editedCellIndex].data.components[z].C = double.Parse(CellEditorC.text);
            map2d[editedCellIndex].data.components[z].Q = double.Parse(CellEditorQ.text);
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

        if (cell.data.components.Count > 0)
        {
            for (int i = 0; i < cell.data.components.Count; i++)
            {
                text += System.Environment.NewLine;
                text += "new component num:\t" + i.ToString("N0") + System.Environment.NewLine;
                text += "type:\t" + cell.data.components[i].type + System.Environment.NewLine;
                text += "m:\t" + cell.data.components[i].m.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "C:\t" + cell.data.components[i].C.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "Q:\t" + cell.data.components[i].Q.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
                text += "Ro:\t" + cell.data.components[i].Ro.ToString("n8").Replace(",", ".") + System.Environment.NewLine;

                double tT = cell.data.components[i].Q / cell.data.components[i].m / cell.data.components[i].C - 273.15;
                text += "->T:\t" + tT.ToString("n8").Replace(",", ".") + System.Environment.NewLine;
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
                map2d[i].data.V = reader.ReadDouble();
                map2d[i].data.transferToUp = reader.ReadDouble();
                map2d[i].data.transferToDown = reader.ReadDouble();
                map2d[i].data.transferToLeft = reader.ReadDouble();
                map2d[i].data.transferToRight = reader.ReadDouble();
                map2d[i].data.transferToUpLeft = reader.ReadDouble();
                map2d[i].data.transferToUpRight = reader.ReadDouble();
                map2d[i].data.transferToDownLeft = reader.ReadDouble();
                map2d[i].data.transferToDownRight = reader.ReadDouble();

                int count = reader.ReadInt32();
                for (int y = 0; y < count; y++)
                {
                    myComponent temp = new myComponent();
                    temp.type = (myComponentType)reader.ReadInt32();
                    temp.m = reader.ReadDouble();
                    temp.C = reader.ReadDouble();
                    temp.Q = reader.ReadDouble();
                    temp.Ro = reader.ReadDouble();
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
    public void SelectElem(int elementNum)
    {
        selectedType = (ModeType)elementNum;
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

                map2d.Add(myclone);
            }
        }
        prefab.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {

    }
}