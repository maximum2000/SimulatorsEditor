using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MyButtonUI : MonoBehaviour
{
    SpriteRenderer sprite;
    // Start is called before the first frame update
    void Start()
    {
        sprite = this.GetComponent<SpriteRenderer>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnEnter()
    {
        sprite.color = new Color(0.7f, 0.7f, 0.7f);
        //Debug.Log("!!!!!");
    }

    public void OnExit()
    {
        sprite.color = new Color(1f, 1f, 1f);
        //Debug.Log("!!!!!");
    }

    public void OnDown()
    {
        sprite.color = new Color(0.5f, 0.5f, 0.5f);
        //Debug.Log("!!!!!");
    }

    public void OnUp()
    {
        sprite.color = new Color(0.6f, 0.6f, 0.6f);
        //Debug.Log("!!!!!");
    }


}
