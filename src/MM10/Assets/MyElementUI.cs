using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MyElementUI : MonoBehaviour
{
    public MM10 mm;
    public MyElementUI thiselement;

    public int index ;

    // Start is called before the first frame update
    void Start()
    {
        
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
}
