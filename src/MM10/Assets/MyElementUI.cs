using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MyElementUI : MonoBehaviour
{
    public MM10 mm;
    public MyElementUI thiselement;

    public int index ;

    public myElementData data;

    // Start is called before the first frame update
    void Start()
    {
        data = new myElementData();
        data.Clear();
        //Debug.Log(data.components.Count);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Click()
    {
        //Debug.Log("!!!!!");
        mm.CellClick(ref thiselement);
    }

    public void Move()
    {
        mm.CellInfo(ref thiselement);

        if (Input.GetMouseButton(0) == false) return;
        //Debug.Log("Move!");
        mm.CellClick(ref thiselement);
    }

    public void ManualUpdate()
    {
        float R = 1f;
        float G = 1f;
        float B = 1f;

        float Gas = 0;
        float Fluid = 0;
        float Solid = 0;
        float Wall = 0;


        if (data.components.Count > 0)
        {
            for (int i = 0; i < data.components.Count; i++)
            {
                if (data.components[i].type== myComponentType.gas)
                {
                    Gas += data.components[i].m;
                }
                if (data.components[i].type == myComponentType.fluid)
                {
                    Fluid += data.components[i].m;
                }
                if (data.components[i].type == myComponentType.solid)
                {
                    Solid += data.components[i].m;
                }
                if (data.components[i].type == myComponentType.wall)
                {
                    Wall += data.components[i].m;
                }
            }

            R = Solid / (Gas + Fluid + Solid);
            G = Gas / (Gas + Fluid + Solid);
            B = Fluid / (Gas + Fluid + Solid);

            if (Wall>0)
            {
                R = 0;
                G = 0;
                B = 0;
            }
        }

        this.gameObject.GetComponent<SpriteRenderer>().color = new Color(R, G, B);
    }
}
