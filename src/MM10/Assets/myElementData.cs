using System.Collections;
using System.Collections.Generic;


public enum myComponentType
{
    none,
    wall,
    gas,
    fluid,
    solid
}


public class myComponent
{
    //����� ����������
    public float m=0;
    //��� ����������
    public myComponentType type = myComponentType.none;
    //������������
    //����������� � ���������� �������, ����������� (����������) ����� � �������� ���������� (���������) �� 1 �������
    //�������� ����������� (����������� ������� ����� ����)
    //� ��� - �������� ������������, �.�. ����������� ������� ����� ���� (����������)
    public float C;
    //������� (�������) , ������
    public float Q;
    //��������� (��� ��������� � ������� ���)
    //��������� � ��������� ���������� ��������, ������������ ��� ��������� ����� ���� � ����������� ���� ����� ������, ��/�3
    public float Ro;

    //����:
    //���� ����� ������ ���������� ������� ������ �������� ������������, ����� �������... ��� ����� ����� ���������
    //���� ����� ���������� �����, ��� �����.. ������������ ���� � ������� ... ����� ��������� �������� � ����������� ����
    //���� ����� ������ � ������� ����������� ... � �� ������������ � �������... ����� ��������� � �� �����������
    //����� ����������� ������������ � ������������� ������� ��� ����������� ��������� (����������� ���� ����������� �������������)
}

public class myElementData
{
    //�������� ����������� � ���� ������
    public List<myComponent> components;
    //����� ������, �3
    public float V=1f;

    //������ �������� ����� �� ����� ���������� [�� ����� q9]
    public float transferToUp = 0;
    public float transferToDown = 0;
    public float transferToLeft = 0;
    public float transferToRight = 0;
    public float transferToUpLeft = 0;
    public float transferToUpRight = 0;
    public float transferToDownLeft = 0;
    public float transferToDownRight = 0;

    //����:
    //���� ��������� ������� �����������, ����� �������������� ������� ������� ��������� ��� ��������� ������� ����������
    //�.�. ����� ��������� ���� � ������ ������������ �������, ������� �������� ����� ��� �� ���������� �������������


    public void Clear()
    {
        components = new List<myComponent>();
        V = 1f;
        transferToUp = 0f;
        transferToDown = 0;
        transferToLeft = 0;
        transferToRight = 0;
        transferToUpLeft = 0;
        transferToUpRight = 0;
        transferToDownLeft = 0;
        transferToDownRight = 0;
    }
}


