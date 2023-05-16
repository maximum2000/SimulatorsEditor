    using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using System.IO;

public class MM10 : MonoBehaviour
{
    private int maxx = 80;
    private int maxy = 80;


    public MyElementUI prefab;
    //public GameObject parent;
    public float dx;
    public float dy;

    public TextMeshPro info_text;

    private List<MyElementUI> map2d;

    public enum ModeType
    {
        none,
        wall,
        gas,
        fluid,
        solid,
        delete
    }

    public ModeType selectedType;


    public void CellClick(ref MyElementUI cell)
    {
        if (selectedType == ModeType.none)
        {
            //cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 0f, 0);
        }
        else if (selectedType == ModeType.gas)
        {
            cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f,1f, 0f);
        }
        else if (selectedType == ModeType.fluid)
        {
            cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(0, 0f, 1f);
        }
        else if (selectedType == ModeType.solid)
        {
            cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(0.5f, 0.5f, 0.5f);
        }
        else if (selectedType == ModeType.wall)
        {
            cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(0f, 0f, 0f);
        }
        else if (selectedType == ModeType.delete)
        {
            cell.gameObject.GetComponent<SpriteRenderer>().color = new Color(1f, 1f, 1f);
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
        text += "x:" + index_x + "\ty:" + index_y;

        info_text.text = text;
    }
    

    // Start is called before the first frame update
    void Start()
    {
        selectedType = ModeType.none;
    }

    public void LoadScene()
    {
        string filename = Path.Combine(Application.streamingAssetsPath, "test.model");
        if (File.Exists(filename) == false) return;


        NewTable();


        BinaryReader reader;
        reader = new BinaryReader(File.Open(filename, FileMode.Open));

        int i = 0;


        while (reader.BaseStream.Position != reader.BaseStream.Length)
        {
            try
            {
                float r = reader.ReadSingle();
                float g = reader.ReadSingle();
                float b = reader.ReadSingle();
                map2d[i].gameObject.GetComponent<SpriteRenderer>().color= new Color(r, g, b);
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
            writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.r);
            writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.g);
            writer.Write(map2d[i].gameObject.GetComponent<SpriteRenderer>().color.b);
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




    public void NewTable()
    {
        prefab.gameObject.SetActive(true);

        //чистим
        if (map2d != null)
        {
            for (int i = 0; i < maxx * maxy; i++)
            {
                Destroy(map2d[i].gameObject);
            }
            map2d.Clear();
        }


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
