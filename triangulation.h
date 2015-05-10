#ifndef MONOTONE_H
#define MONOTONE_H

#include <cstdio>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

#define LEFTSIDE false
#define RIGHTSIDE true

using namespace std;

#include "pcustomtypes.h"
#include "triangulation.h"


class Triangulation
{
private:
    priority_queue<Vertex, vector<Vertex>, Vertex> pq;

    vector<Edges> diagonals;
    vector<Edges> edgeList;
    vector<bool> side;

    int totalPoint;
    vector<Vertex> vertex;
    stack<Vertex> stk;

    void takeInput();
    void drawOriginalPolygon();
    void drawDiagonals();
    void triangulate();
    bool isDiagonalInside(Vertex s, Vertex m, Vertex e);
    //int getVertexType(int id);
    //void vertexHandler(int vertexId, int vertexType, double y);
    void drawChar(int x, int y, char *str);
public:
    Triangulation();
    void draw();
};

Triangulation :: Triangulation()
{
    takeInput();
    triangulate();
}
bool Triangulation :: isDiagonalInside(Vertex s, Vertex m, Vertex e)
{
    int prev,next;
    //printf("testing %d,%d,%d\n",s.index,m.index,e.index);

    if(s.index>e.index)
    {
        Vertex t=s;
        s=e;
        e=t;
    }

    printf("testing %d,%d,%d\n",s.index,m.index,e.index);

    Vector3 prevVector(s.x-m.x,s.y-m.y,0);
    Vector3 nextVector(e.x-m.x,e.y-m.y,0);
    //Vector3 nextVector(vertex[next].x-vertex[id].x,vertex[next].y-vertex[id].y,0);
    //Vector3 nextVector(v[next].x-v[id].x,v[next].y-v[id].y,0);
    if(prevVector.zcross(nextVector)<0)//internal angle greater than 180
    {
        printf("diagonal outside\n");
        return false;
    }
    else
    {
        printf("diagonal inside\n");
        return true;
    }
}

void Triangulation :: drawChar(int x, int y, char *string)
{
    float r=0.8;
    float g=0.8;
    float b=0.8;
    //int font=GLUT_BITMAP_TIMES_ROMAN_10;
    //GLUT_BITMAP_TIMES_ROMAN_10
    glColor3f( r, g, b );
    glRasterPos2f(x, y);
    int len, i;
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        //printf("%d--",i);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}

void Triangulation :: takeInput()
{
    FILE *ifp;
    int maxPoint=0,minPoint=0;
    ifp = fopen("G:\\Dropbox\\open gl\\Triangulation\\input.txt","r");//for Desktop

    fscanf(ifp,"%d",&totalPoint);
    double a,b;
    for(int i=0; i<totalPoint; i++)
    {
        fscanf(ifp,"%lf%lf",&a,&b);
        vertex.push_back(Vertex(i,a,b));
        side.push_back(LEFTSIDE);
        if(vertex[i]>vertex[maxPoint])
        {
            maxPoint=i;
        }
        else if(vertex[i]<vertex[minPoint])
        {
            minPoint=i;
        }
    }
    printf("max %d, min %d\n",maxPoint,minPoint);
    int i=maxPoint;
    while(i!=minPoint)
    {
        side[i]=RIGHTSIDE;
        i++;
        if(i==totalPoint)
        {
            i=0;
        }
    }

/// list of edges -> 1,0 - 2,1 - ... - 0,last
    edgeList.push_back(Edges(vertex[0],vertex[totalPoint-1]));
//    edgePointer.push_back(it);
    for(int i=0; i<totalPoint-1; i++)
    {
        edgeList.push_back(Edges(vertex[i+1],vertex[i]));
//        edgePointer.push_back(it);
    }
    fclose(ifp);
}
void Triangulation :: drawDiagonals()
{
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    {
        for(int i=0; i<diagonals.size(); i++)
        {
            glVertex2f(diagonals[i].a.x,diagonals[i].a.y);
            glVertex2f(diagonals[i].b.x,diagonals[i].b.y);
        }
    }
    glEnd();
}
void Triangulation :: drawOriginalPolygon()
{
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINE_LOOP);
    {
        for(unsigned int i=0; i<vertex.size(); i++)
        {
            glVertex2f(vertex[i].x,vertex[i].y);
        }
    }
    glEnd();

    char num[8];

    for(int i=0; i<vertex.size(); i++)
    {
        itoa(i,num,10);
        if(side[i]==LEFTSIDE)
        {
            strcat(num,"-L");
        }
        else
        {
            strcat(num,"-R");
        }
        drawChar(vertex[i].x,vertex[i].y,num);
    }
}


void Triangulation :: triangulate()
{
    diagonals.clear();
    vector<Vertex> sortedV(vertex);
    sort(sortedV.begin(),sortedV.end(),verSortComp);
    stk.push(sortedV[0]);
    printf("pushing %d\n",sortedV[0]);
    stk.push(sortedV[1]);
    printf("pushing %d\n",sortedV[1]);
    int lmt=totalPoint-1;
    Vertex stackTop,lastPopped;
    for (int j=2; j<lmt; j++)
    {
        printf("\ncurrent point %d\n",sortedV[j].index);
        stackTop = stk.top();
        if(side[sortedV[j].index]!=side[stackTop.index])
        {
            printf("different chain %d,%d\n",sortedV[j].index,stackTop.index);
            while(!stk.empty())
            {
                stackTop=stk.top();
                stk.pop();
                printf("popped %d\n",stackTop.index);
                if(!stk.empty())
                {
                    diagonals.push_back(Edges(sortedV[j],stackTop));
                    printf("added diagonal %d to %d\n",sortedV[j].index,stackTop.index);
                }
            }
            stk.push(sortedV[j-1]);
            printf("pushing %d\n",sortedV[j-1]);
            stk.push(sortedV[j]);
            printf("pushing %d\n",sortedV[j]);
        }

        else
        {
            printf("same chain %d,%d\n",sortedV[j].index,stackTop.index);
            printf("popping top\n");
            lastPopped=stk.top();
            stk.pop();
            printf("popped %d\n",lastPopped.index);
            if(!stk.empty())
            {
                stackTop=stk.top();
            }

            while(!stk.empty() && isDiagonalInside(sortedV[j],lastPopped,stackTop))
            {
                stk.pop();
                printf("popped %d\n",stackTop.index);
                diagonals.push_back(Edges(sortedV[j],stackTop));
                printf("adding diagonal %d to %d\n",sortedV[j].index,stackTop.index);
                lastPopped=stackTop;
                if(!stk.empty())
                {
                    stackTop=stk.top();
                }
            }

            stk.push(lastPopped);
            printf("pushing %d\n",lastPopped.index);
            stk.push(sortedV[j]);
            printf("pushing %d\n",sortedV[j].index);
        }
    }
    if(!stk.empty())
    {
        stk.pop();
    }
    while(!stk.empty())
    {
        stackTop=stk.top();
        stk.pop();
        if(!stk.empty())
        {
            diagonals.push_back(Edges(sortedV[totalPoint-1],stackTop));
        }
    }
}

void Triangulation :: draw()
{
    drawOriginalPolygon();
    drawDiagonals();
}
#endif // MONOTONE_H
