    using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MM10 : MonoBehaviour
{
    public MyElementUI prefab;
    //public GameObject parent;
    public float dx;
    public float dy;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    public void NewTable()
    {
        for (int x = 0; x < 80; x++)
        {
            for (int y = 0; y < 80; y++)
            {
                MyElementUI myclone = Instantiate(prefab);
                myclone.gameObject.transform.localPosition = new Vector3(dx + x * 0.11f, dy + y * 0.11f, 0);
                myclone.gameObject.GetComponent<SpriteRenderer>().color = new Color((float)x/40f, (float)y/40f, 0);

                // myclone.transform.parent = parent.transform;

                //MyElementUI myclone = Instantiate(prefab, new Vector3(x * 20.0f, y * 20.0f, 0), Quaternion.identity);
                //myclone.transform.parent = parent.transform;


            }
        }
        prefab.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
