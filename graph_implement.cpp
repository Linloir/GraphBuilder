#include "graph_implement.h"
#include <QDebug>

AbstractGraph::~AbstractGraph(){}

void ALVex::visit(){
    if(visited)
        return;
    info->gVex->visit(true);
    visited = true;
}

void ALVex::access(const QString &hint){
    info->gVex->access(hint);
}

void ALArc::visit(){
    gArc->visit(true);
}

void ALArc::access(){
    gArc->access();
}

ALGraph::~ALGraph(){
    int i = vexList.size() - 1;
    while(i >= 0)
        DelVex(i--);
    vexList.clear();
}

void ALGraph::AddVex(MyGraphicsVexItem *gvex){
    ALVex newVex(gvex);
    vexList.push_back(newVex);
}

void ALGraph::AddVex(VexInfo *info){
    ALVex newVex(info);
    vexList.push_back(newVex);
}

void ALGraph::AddArc(MyGraphicsLineItem *garc, int weight){
    int strtVex = GetIdOf(garc->stVex());
    int endVex = GetIdOf(garc->edVex());

    ALArc *temp = vexList[strtVex].firstArc;
    ALArc *newArc = new ALArc(garc, endVex, temp);
    newArc->weight = weight;
    vexList[strtVex].firstArc = newArc;

    if(type == UDG){
        temp = vexList[endVex].firstArc;
        newArc = new ALArc(garc, strtVex, temp);
        vexList[endVex].firstArc = newArc;
    }
}

void ALGraph::DelVex(MyGraphicsVexItem *gvex){
    int vexID = GetIdOf(gvex);
    DelVex(vexID);
}

void ALGraph::DelVex(int vexID){
    //Delete out arc
    ALArc *curArc = vexList[vexID].firstArc;
    while(curArc != nullptr){
        ALArc *next = curArc->nextArc;
        delete curArc;
        curArc = next;
    }
    //Delete in arc and adjust arcs
    for(int i = 0; i < vexList.size(); i++){
        if(i == vexID)  continue;
        ALArc *dummyHead = new ALArc(nullptr, 0, vexList[i].firstArc);
        ALArc *preArc = dummyHead;
        while(preArc->nextArc != nullptr){
            if(preArc->nextArc->eVexID == vexID){
                ALArc *next = preArc->nextArc;
                preArc->nextArc = next->nextArc;
                delete next;
                continue;
            }
            if(preArc->nextArc->eVexID > vexID)
                preArc->nextArc->eVexID--;
            preArc = preArc->nextArc;
        }
        vexList[i].firstArc = dummyHead->nextArc;
        delete dummyHead;
    }
    vexList.erase(vexList.begin() + vexID);
}

void ALGraph::DelArc(MyGraphicsLineItem *garc){
    int sVex = GetIdOf(garc->stVex());
    int eVex = GetIdOf(garc->edVex());
    DelArc(sVex, eVex);
}

void ALGraph::DelArc(int sVexID, int eVexID){
    //Delete sVex -> eVex
    if(vexList[sVexID].firstArc != nullptr){
        if(vexList[sVexID].firstArc->eVexID == eVexID){
            ALArc *awaitDel = vexList[sVexID].firstArc;
            vexList[sVexID].firstArc = awaitDel->nextArc;
            delete awaitDel;
        }
        else{
            ALArc *preArc = vexList[sVexID].firstArc;
            while(preArc->nextArc != nullptr && preArc->nextArc->eVexID != eVexID)
                preArc = preArc->nextArc;
            if(preArc->nextArc != nullptr){
                ALArc *awaitDel = preArc->nextArc;
                preArc->nextArc = awaitDel->nextArc;
                delete awaitDel;
            }
        }
    }
    //Delete eVex -> sVex
    if(type == UDG && vexList[eVexID].firstArc != nullptr){
        if(vexList[eVexID].firstArc->eVexID == sVexID){
            ALArc *awaitDel = vexList[eVexID].firstArc;
            vexList[eVexID].firstArc = awaitDel->nextArc;
            delete awaitDel;
        }
        else{
            ALArc *preArc = vexList[eVexID].firstArc;
            while(preArc->nextArc != nullptr && preArc->nextArc->eVexID != sVexID)
                preArc = preArc->nextArc;
            if(preArc->nextArc != nullptr){
                ALArc *awaitDel = preArc->nextArc;
                preArc->nextArc = awaitDel->nextArc;
                delete awaitDel;
            }
        }
    }
}

int ALGraph::GetIdOf(MyGraphicsVexItem *gvex){
    int i = 0;
    while(i < vexList.size() && !vexList[i].info->gVex->equalTo(gvex))
        i++;
    return i == vexList.size() ? -1 : i;
}

ALArc* ALGraph::FindArc(int sID, int eID){
    if(sID < 0 || sID >= vexList.size())
        return nullptr;
    ALArc *p = vexList[sID].firstArc;
    while(p != nullptr){
        if(p->eVexID == eID)
            return p;
        p = p->nextArc;
    }
    return nullptr;
}

void ALGraph::SetWeight(MyGraphicsLineItem *garc, int weight){
    int strtVex = GetIdOf(garc->stVex());
    int endVex = GetIdOf(garc->edVex());
    ALArc *p = vexList[strtVex].firstArc;
    while(p != nullptr){
        if(p->eVexID == endVex){
            p->weight = weight;
            p->gArc->setText(QString::asprintf("%d", weight));
        }
        p = p->nextArc;
    }
    if(type == UDG){
        p = vexList[endVex].firstArc;
        while(p != nullptr){
            if(p->eVexID == strtVex){
                p->weight = weight;
            }
            p = p->nextArc;
        }
    }
}

void ALGraph::ConvertType(int _type){
    if(_type == type)   return;
    type = _type;
    if(type == UDG){
        for(int i = 0; i < vexList.size(); i++){
            ALArc *dummyHead = new ALArc(nullptr, i, vexList[i].firstArc);
            ALArc *pre = dummyHead;
            while(pre->nextArc != nullptr){
                if(pre->nextArc->eVexID == GetIdOf(pre->nextArc->gArc->edVex())){
                    ALArc *temp = vexList[pre->nextArc->eVexID].firstArc;
                    vexList[pre->nextArc->eVexID].firstArc = new ALArc(pre->nextArc->gArc, i, temp);
                    pre->nextArc->gArc->setDirection(false);
                }
                pre = pre->nextArc;
            }
            delete dummyHead;
        }
    }
    else{
        for(int i = 0; i < vexList.size(); i++){
            ALArc *dummyHead = new ALArc(nullptr, i, vexList[i].firstArc);
            ALArc *pre = dummyHead;
            while(pre->nextArc != nullptr){
                if(pre->nextArc->eVexID != GetIdOf(pre->nextArc->gArc->edVex())){
                    ALArc *temp = pre->nextArc;
                    pre->nextArc = temp->nextArc;
                    temp->gArc->setDirection(true);
                    delete temp;
                    continue;
                }
                pre = pre->nextArc;
            }
            vexList[i].firstArc = dummyHead->nextArc;
            delete dummyHead;
        }
    }
}

void ALGraph::ClearVisit(){
    for(int i = 0; i < vexList.size(); i++){
        vexList[i].visited = false;
        vexList[i].info->gVex->visit(false);
    }
}

void ALGraph::ResetDistance(){
    for(int i = 0; i < vexList.size(); i++){
        vexList[i].info->strtVexInfo = nullptr;
        vexList[i].info->distance = VexInfo::INF;
        vexList[i].info->preVexID = -1;
        vexList[i].info->gVex->access("", false);
    }
}

void ALGraph::DFS(int strtID, bool generateForest){
    if(strtID == -1)
        return;
    vector<int> awaitVexList;
    vector<ALArc*> awaitArcList;
    awaitVexList.push_back(strtID);
    while(awaitVexList.size() > 0){
        int nextVex = awaitVexList.back();
        ALArc *nextArc = awaitArcList.size() > 0 ? awaitArcList.back() : nullptr;
        awaitVexList.pop_back();
        if(nextArc)
            awaitArcList.pop_back();
        for(ALArc *p = vexList[nextVex].firstArc; p != nullptr; p = p->nextArc){
            if(vexList[p->eVexID].visited == false){
                awaitVexList.push_back(p->eVexID);
                awaitArcList.push_back(p);
                if(type == UDG && GetIdOf(p->gArc->edVex()) != p->eVexID)
                    p->gArc->reverseDirection();
            }
        }
        if(nextArc && !vexList[nextArc->eVexID].visited)
            nextArc->visit();
        vexList[nextVex].visit();
    }
    if(generateForest){
        for(int i = 0; i < vexList.size(); i++){
            if(vexList[i].visited)  continue;
            awaitVexList.clear();
            awaitArcList.clear();
            awaitVexList.push_back(i);
            while(awaitVexList.size() > 0){
                int nextVex = awaitVexList.back();
                ALArc *nextArc = awaitArcList.size() > 0 ? awaitArcList.back() : nullptr;
                awaitVexList.pop_back();
                if(nextArc)
                    awaitArcList.pop_back();
                for(ALArc *p = vexList[nextVex].firstArc; p != nullptr; p = p->nextArc){
                    if(vexList[p->eVexID].visited == false){
                        awaitVexList.push_back(p->eVexID);
                        awaitArcList.push_back(p);
                        if(type == UDG && GetIdOf(p->gArc->edVex()) != p->eVexID)
                            p->gArc->reverseDirection();
                    }
                }
                if(nextArc && !vexList[nextArc->eVexID].visited)
                    nextArc->visit();
                vexList[nextVex].visit();
            }
        }
    }
}

void ALGraph::BFS(int strtID, bool generateForest){
    if(strtID == -1)
        return;
    vector<int> awaitVexList;
    vector<ALArc*> awaitArcList;
    awaitVexList.push_back(strtID);
    while(awaitVexList.size() > 0){
        int nextVex = awaitVexList[0];
        ALArc *nextArc = awaitArcList.size() > 0 ? awaitArcList[0] : nullptr;
        awaitVexList.erase(awaitVexList.begin());
        if(nextArc)
            awaitArcList.erase(awaitArcList.begin());
        for(ALArc *p = vexList[nextVex].firstArc; p != nullptr; p = p->nextArc){
            if(vexList[p->eVexID].visited == false){
                awaitVexList.push_back(p->eVexID);
                awaitArcList.push_back(p);
                if(type == UDG && GetIdOf(p->gArc->edVex()) != p->eVexID)
                    p->gArc->reverseDirection();
            }
        }
        if(nextArc && !vexList[nextArc->eVexID].visited)
            nextArc->visit();
        vexList[nextVex].visit();
    }
    if(generateForest){
        for(int i = 0; i < vexList.size(); i++){
            if(vexList[i].visited)  continue;
            awaitVexList.clear();
            awaitArcList.clear();
            awaitVexList.push_back(i);
            while(awaitVexList.size() > 0){
                int nextVex = awaitVexList[0];
                ALArc *nextArc = awaitArcList.size() > 0 ? awaitArcList[0] : nullptr;
                awaitVexList.erase(awaitVexList.begin());
                if(nextArc)
                    awaitArcList.erase(awaitArcList.begin());
                for(ALArc *p = vexList[nextVex].firstArc; p != nullptr; p = p->nextArc){
                    if(vexList[p->eVexID].visited == false){
                        awaitVexList.push_back(p->eVexID);
                        awaitArcList.push_back(p);
                        if(type == UDG && GetIdOf(p->gArc->edVex()) != p->eVexID)
                            p->gArc->reverseDirection();
                    }
                }
                if(nextArc && !vexList[nextArc->eVexID].visited)
                    nextArc->visit();
                vexList[nextVex].visit();
            }
        }
    }
}

void ALGraph::Dijkstra(int strtID){
    //Clear previous result
    ClearVisit();
    ResetDistance();
    //Set start vex info to all vertexes
    for(int i = 0; i < vexList.size(); i++)
        vexList[i].info->strtVexInfo = vexList[strtID].info;
    //Start dijkstra
    vexList[strtID].info->distance = 0;
    vexList[strtID].access("strt");
    while(true){
        //Find next
        int minVexID = -1;
        for(int i = 0; i < vexList.size(); i++){
            if(vexList[i].visited || vexList[i].info->distance == VexInfo::INF)
                continue;
            if(minVexID == -1)
                minVexID = i;
            else if(vexList[i].info->distance < vexList[minVexID].info->distance)
                minVexID = i;
        }
        if(minVexID == -1)
            break;
        //Set visit to edge and vex
        ALArc *edge = FindArc(vexList[minVexID].info->preVexID, minVexID);
        if(edge){
            if(type == UDG && GetIdOf(edge->gArc->edVex()) != minVexID)
                edge->gArc->reverseDirection();
            edge->visit();
        }
        vexList[minVexID].visit();
        //Find adjacent
        for(ALArc *p = vexList[minVexID].firstArc; p != nullptr; p = p->nextArc){
            if(!vexList[p->eVexID].visited){
                if(GetIdOf(p->gArc->edVex()) != p->eVexID)
                    p->gArc->reverseDirection();
                p->access();
                if(vexList[p->eVexID].info->distance == VexInfo::INF ||
                        vexList[p->eVexID].info->distance > vexList[minVexID].info->distance + p->weight){
                    vexList[p->eVexID].info->preVexID = minVexID;
                    vexList[p->eVexID].info->distance = vexList[minVexID].info->distance + p->weight;
                    vexList[p->eVexID].access(QString::asprintf("%d", vexList[p->eVexID].info->distance));
                }
            }
        }
    }
}

AMLGraph* ALGraph::ConvertToAML(){
    AMLGraph *converted = new AMLGraph(type);
    for(int i = 0; i < vexList.size(); i++){
        converted->AddVex(vexList[i].info);
    }
    for(int i = 0; i < vexList.size(); i++){
        ALArc *p = vexList[i].firstArc;
        while(p != nullptr){
            if(type == DG || (type == UDG && i == GetIdOf(p->gArc->edVex())))
                converted->AddArc(p->gArc, p->weight);
            p = p->nextArc;
        }
    }
    return converted;
}

/**************************************************************************************/

void AMLVex::visit(){
    if(visited)
        return;
    info->gVex->visit(true);
    visited = true;
}

void AMLVex::access(const QString &hint){
    info->gVex->access(hint);
}

void AMLArc::visit(){
    gArc->visit(true);
}

void AMLArc::access(){
    gArc->access();
}

AMLGraph::~AMLGraph(){
    int i = outVexList.size() - 1;
    while(i >= 0){
        DelVex(i--);
    }
    outVexList.clear();
    if(type == DG)
        inVexList.clear();
}

void AMLGraph::AddVex(MyGraphicsVexItem *gvex){
    AMLVex newVex(gvex);
    outVexList.push_back(newVex);

    if(type == DG){
        inVexList.push_back(newVex);
    }
}

void AMLGraph::AddVex(VexInfo *info){
    AMLVex newVex(info);
    outVexList.push_back(newVex);

    if(type == DG){
        inVexList.push_back(newVex);
    }
}

void AMLGraph::AddArc(MyGraphicsLineItem *garc, int weight){
    int strtVex = GetIdOf(garc->stVex());
    int endVex = GetIdOf(garc->edVex());

    AMLArc *nextOutArc = outVexList[strtVex].firstArc;
    AMLArc *nextInArc = type == DG ? inVexList[endVex].firstArc : outVexList[endVex].firstArc;
    AMLArc *newArc = new AMLArc(garc, strtVex, endVex, nextOutArc, nextInArc);
    newArc->weight = weight;
    outVexList[strtVex].firstArc = newArc;
    if(type == DG)
        inVexList[endVex].firstArc = newArc;
    else
        outVexList[endVex].firstArc = newArc;
}

void AMLGraph::DelVex(MyGraphicsVexItem *gvex){
    int vexID = GetIdOf(gvex);
    DelVex(vexID);
}

void AMLGraph::DelVex(int vexID){
    if(type == DG){
        //Delete out arc
        AMLArc *outArc = outVexList[vexID].firstArc;
        while(outArc != nullptr){
            AMLArc *dummyHead = new AMLArc(nullptr, 0, 0, nullptr, inVexList[outArc->inVexID].firstArc);
            AMLArc *preInArc = dummyHead;
            while(preInArc->nextInArc != nullptr){
                if(preInArc->nextInArc->outVexID == vexID){
                    AMLArc *next = preInArc->nextInArc;
                    preInArc->nextInArc = next->nextInArc;
                    delete next;
                }
                else
                    preInArc = preInArc->nextInArc;
            }
            inVexList[outArc->inVexID].firstArc = dummyHead->nextInArc;
            delete dummyHead;
            outArc = outArc->nextOutArc;
        }
        //Delete in arc and adjust ID
        for(int i = 0; i < outVexList.size(); i++){
            if(i == vexID)  continue;
            AMLArc *dummyHead = new AMLArc(nullptr, 0, 0, outVexList[i].firstArc, nullptr);
            AMLArc *preOutArc = dummyHead;
            while(preOutArc->nextOutArc != nullptr){
                if(preOutArc->nextOutArc->inVexID == vexID){
                    AMLArc *next = preOutArc->nextOutArc;
                    preOutArc->nextOutArc = next->nextOutArc;
                    delete next;
                    continue;
                }
                if(preOutArc->nextOutArc->inVexID > vexID)
                    preOutArc->nextOutArc->inVexID--;
                if(preOutArc->nextOutArc->outVexID > vexID)
                    preOutArc->nextOutArc->outVexID--;
                preOutArc = preOutArc->nextOutArc;
            }
            outVexList[i].firstArc = dummyHead->nextOutArc;
            delete dummyHead;
        }
        outVexList.erase(outVexList.begin() + vexID);
        inVexList.erase(inVexList.begin() + vexID);
    }
    else{
        //Traverse all and adjust ID
        AMLArc *p = outVexList[vexID].firstArc;
        while(p != nullptr){
            if(p->outVexID == vexID){
                AMLArc *temp = p->nextOutArc;
                p->nextOutArc = nullptr;
                p = temp;
            }
            else{
                AMLArc *temp = p->nextInArc;
                p->nextInArc = nullptr;
                p = temp;
            }
        }
        outVexList[vexID].firstArc = nullptr;
        for(int i = 0; i < outVexList.size(); i++){
            if(i == vexID)  continue;
            AMLArc *dummyHead = new AMLArc(nullptr, i, -1, outVexList[i].firstArc, nullptr);
            AMLArc *preArc = dummyHead;
            AMLArc *nextArc = preArc->nextOutArc;
            while(nextArc != nullptr){
                if(nextArc->inVexID == vexID || nextArc->outVexID == vexID){
                    if(preArc->outVexID == i || preArc->outVexID == -i){
                        if(nextArc->outVexID == i){
                            preArc->nextOutArc = nextArc->nextOutArc;
                            nextArc->nextOutArc = nullptr;
                            delete nextArc;
                        }
                        else{
                            preArc->nextOutArc = nextArc->nextInArc;
                            nextArc->nextInArc = nullptr;
                            delete nextArc;
                        }
                    }
                    else{
                        if(nextArc->outVexID == i){
                            preArc->nextInArc = nextArc->nextOutArc;
                            nextArc->nextOutArc = nullptr;
                            delete nextArc;
                        }
                        else{
                            preArc->nextInArc = nextArc->nextInArc;
                            nextArc->nextInArc = nullptr;
                            delete nextArc;
                        }
                    }
                    nextArc = (preArc->outVexID == i || preArc->outVexID == -i) ? preArc->nextOutArc : preArc->nextInArc;
                    continue;
                }
                if(preArc->inVexID > vexID)
                    preArc->inVexID = - preArc->inVexID;
                else if(preArc->inVexID < 0)
                    preArc->inVexID = - preArc->inVexID - 1;
                if(preArc->outVexID > vexID)
                    preArc->outVexID = - preArc->outVexID;
                else if(preArc->outVexID < 0)
                    preArc->outVexID = - preArc->outVexID - 1;
                preArc = nextArc;
                nextArc = (preArc->outVexID == i || preArc->outVexID == -i) ? preArc->nextOutArc : preArc->nextInArc;

                /********************************************************************************************/
                //Issue #1
                /*How this causes issue:
                 *   Example： when vexID is 1
                 *   preArc = nextArc => preArc: -3 -> -2
                 *   nextArc = ... => nextArc : 3 -> 1
                 *   Adjust pre Arc to => preArc: 2 -> 1
                 *   In next loop: nextArc->inVexID == 1 => judge if preArc->nextOutArc == 3 (in fact it is)
                 *   preArc->nextOutArc != 3 -> wrongly connect next out arc of 3 -> 1 to next out arc of 2 -> 1
                 *   which leads to wrong operations
                 */

                //if(preArc->inVexID > vexID)
                //    preArc->inVexID = - preArc->inVexID;
                //else if(preArc->inVexID < 0)
                //    preArc->inVexID = - preArc->inVexID - 1;
                //if(preArc->outVexID > vexID)
                //    preArc->outVexID = - preArc->outVexID;
                //else if(preArc->outVexID < 0)
                //    preArc->outVexID = - preArc->outVexID - 1;

                /********************************************************************************************/

            }
            if(preArc->inVexID > vexID)
                preArc->inVexID = - preArc->inVexID;
            else if(preArc->inVexID < 0)
                preArc->inVexID = - preArc->inVexID - 1;
            if(preArc->outVexID > vexID)
                preArc->outVexID = - preArc->outVexID;
            else if(preArc->outVexID < 0)
                preArc->outVexID = - preArc->outVexID - 1;
            outVexList[i].firstArc = dummyHead->nextOutArc;
            delete dummyHead;
        }
        //Delete vexID
        outVexList.erase(outVexList.begin() + vexID);
    }
}

void AMLGraph::DelArc(MyGraphicsLineItem *garc){
    int sVex = GetIdOf(garc->stVex());
    int eVex = GetIdOf(garc->edVex());
    DelArc(sVex, eVex);
}

void AMLGraph::DelArc(int sVexID, int eVexID){
    if(type == DG){
        AMLArc *dummyHead;
        AMLArc *preArc;
        //Delete sVex -> eVex
        dummyHead = new AMLArc(nullptr, sVexID, -1, outVexList[sVexID].firstArc, nullptr);
        preArc = dummyHead;
        while(preArc->nextOutArc != nullptr){
            if(preArc->nextOutArc->inVexID == eVexID){
                preArc->nextOutArc = preArc->nextOutArc->nextOutArc;
                continue;
            }
            else
                preArc = preArc->nextOutArc;
        }
        outVexList[sVexID].firstArc = dummyHead->nextOutArc;
        delete dummyHead;
        //Delete eVex -> sVex
        dummyHead = new AMLArc(nullptr, -1, eVexID, nullptr, inVexList[eVexID].firstArc);
        preArc = dummyHead;
        while(preArc->nextInArc != nullptr){
            if(preArc->nextInArc->outVexID == sVexID){
                AMLArc *awaitDel = preArc->nextInArc;
                preArc->nextInArc = preArc->nextInArc->nextInArc;
                delete awaitDel;
                continue;
            }
            else
                preArc = preArc->nextInArc;
        }
        inVexList[eVexID].firstArc = dummyHead->nextInArc;
        delete dummyHead;
    }
    else{
        AMLArc *dummyHead;
        AMLArc *preArc;
        AMLArc *nextArc;
        //Delete sVex -> eVex
        dummyHead = new AMLArc(nullptr, sVexID, -1, outVexList[sVexID].firstArc, nullptr);
        preArc = dummyHead;
        nextArc = outVexList[sVexID].firstArc;
        while(nextArc != nullptr){
            if(nextArc->inVexID == eVexID || nextArc->outVexID == eVexID){
                if(preArc->outVexID == sVexID){
                    if(nextArc->outVexID == sVexID){
                        preArc->nextOutArc = nextArc->nextOutArc;
                        nextArc->nextOutArc = nullptr;
                    }
                    else{
                        preArc->nextOutArc = nextArc->nextInArc;
                        nextArc->nextInArc = nullptr;
                    }
                }
                else{
                    if(nextArc->outVexID == sVexID){
                        preArc->nextInArc = nextArc->nextOutArc;
                        nextArc->nextOutArc = nullptr;
                    }
                    else{
                        preArc->nextInArc = nextArc->nextInArc;
                        nextArc->nextInArc = nullptr;
                    }
                }
                nextArc = preArc->outVexID == sVexID ? preArc->nextOutArc : preArc->nextInArc;
                continue;
            }
            else{
                preArc = nextArc;
                nextArc = preArc->outVexID == sVexID ? preArc->nextOutArc : preArc->nextInArc;
            }
        }
        outVexList[sVexID].firstArc = dummyHead->nextOutArc;
        delete dummyHead;
        //Delete eVex -> sVex and release arc
        dummyHead = new AMLArc(nullptr, -1, eVexID, nullptr, outVexList[eVexID].firstArc);
        preArc = dummyHead;
        nextArc = outVexList[eVexID].firstArc;
        while(nextArc != nullptr){
            if(nextArc->inVexID == sVexID || nextArc->outVexID == sVexID){
                if(preArc->outVexID == eVexID){
                    if(nextArc->outVexID == eVexID){
                        preArc->nextOutArc = nextArc->nextOutArc;
                        delete nextArc;
                    }
                    else{
                        preArc->nextOutArc = nextArc->nextInArc;
                        delete nextArc;
                    }
                }
                else{
                    if(nextArc->outVexID == eVexID){
                        preArc->nextInArc = nextArc->nextOutArc;
                        delete nextArc;
                    }
                    else{
                        preArc->nextInArc = nextArc->nextInArc;
                        delete nextArc;
                    }
                }
                nextArc = preArc->outVexID == eVexID ? preArc->nextOutArc : preArc->nextInArc;
                continue;
            }
            else{
                preArc = nextArc;
                nextArc = preArc->outVexID == eVexID ? preArc->nextOutArc : preArc->nextInArc;
            }
        }
        outVexList[eVexID].firstArc = dummyHead->nextInArc;
        delete dummyHead;
    }
}

int AMLGraph::GetIdOf(MyGraphicsVexItem *gvex){
    int i = 0;
    while(i < outVexList.size() && outVexList[i].info->gVex != gvex)
        i++;
    return i == outVexList.size() ? -1 : i;
}

AMLArc* AMLGraph::FindArc(int strtID, int endID){
    if(strtID < 0 || strtID >= outVexList.size())
        return nullptr;
    AMLArc *p = outVexList[strtID].firstArc;
    while(p != nullptr){
        if(p->outVexID == endID || p->inVexID == endID)
            return p;
        p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc;
    }
    return nullptr;
}

void AMLGraph::SetWeight(MyGraphicsLineItem *garc, int weight){
    int strtVex = GetIdOf(garc->stVex());
    int endVex = GetIdOf(garc->edVex());
    AMLArc *p = outVexList[strtVex].firstArc;
    while(p != nullptr){
        if(p->inVexID == endVex || p->outVexID == endVex){
            p->weight = weight;
            p->gArc->setText(QString::asprintf("%d", weight));
        }
        p = p->inVexID == strtVex ? p->nextInArc : p->nextOutArc;
    }
    if(type == DG){
        p = inVexList[endVex].firstArc;
        while(p != nullptr){
            if(p->inVexID == strtVex || p->outVexID == strtVex){
                p->weight = weight;
                p->gArc->setText(QString::asprintf("%d", weight));
            }
            p = p->inVexID == endVex ? p->nextInArc : p->nextOutArc;
        }
    }
}

void AMLGraph::ConvertType(int _type){
    if(_type == type)   return;
    type = _type;
    if(type == UDG){
        for(int i = 0; i < inVexList.size(); i++){
            AMLArc *p = inVexList[i].firstArc;
            while(p != nullptr && p->nextInArc != nullptr){
                p->gArc->setDirection(false);
                p = p->nextInArc;
            }
            AMLArc *temp = outVexList[i].firstArc;
            if(p){
                outVexList[i].firstArc = inVexList[i].firstArc;
                p->nextInArc = temp;
            }
            while(temp != nullptr){
                temp->gArc->setDirection(false);
                temp = temp->nextOutArc;
            }
        }
    }
    else{
        inVexList.clear();
        inVexList.assign(outVexList.begin(), outVexList.end());
        for(int i = 0; i < inVexList.size(); i++)
            inVexList[i].firstArc = nullptr;
        for(int i = 0; i < outVexList.size(); i++){
            AMLArc *dummyHead = new AMLArc(nullptr, i, -1, outVexList[i].firstArc, nullptr);
            AMLArc *pre = dummyHead;
            AMLArc *next = outVexList[i].firstArc;
            while(next != nullptr){
                next->gArc->setDirection(true);
                if(next->outVexID != i){
                    //A reversed edge
                    pre->nextOutArc = next->nextInArc;
                    AMLArc *temp = inVexList[next->inVexID].firstArc;
                    inVexList[next->inVexID].firstArc = next;
                    next->nextInArc = temp;
                    next = pre->nextOutArc;
                    continue;
                }
                pre = next;
                next = pre->nextOutArc;
            }
            outVexList[i].firstArc = dummyHead->nextOutArc;
            delete dummyHead;
        }
    }
}

void AMLGraph::ClearVisit(){
    for(int i = 0; i < outVexList.size(); i++){
        outVexList[i].visited = false;
        outVexList[i].info->gVex->visit(false);
    }
    if(type == DG)
        for(int i = 0; i < inVexList.size(); i++)
            inVexList[i].visited = false;
}

void AMLGraph::ResetDistance(){
    for(int i = 0; i < outVexList.size(); i++){
        outVexList[i].info->strtVexInfo = nullptr;
        outVexList[i].info->distance = VexInfo::INF;
        outVexList[i].info->preVexID = -1;
        outVexList[i].info->gVex->access("", false);
    }
    if(type == DG)
        for(int i = 0; i < outVexList.size(); i++){
            inVexList[i].info->strtVexInfo = nullptr;
            inVexList[i].info->distance = VexInfo::INF;
            inVexList[i].info->preVexID = -1;
        }
}

void AMLGraph::DFS(int strtID, bool generateForest){
    if(strtID == -1)
        return;
    vector<int> awaitVexList;
    vector<AMLArc*>awaitArcList;
    awaitVexList.push_back(strtID);
    while(awaitVexList.size() > 0){
        int nextVex = awaitVexList.back();
        AMLArc *nextArc = awaitArcList.size() > 0 ? awaitArcList.back() : nullptr;
        awaitVexList.pop_back();
        if(nextArc)
            awaitArcList.pop_back();
        for(AMLArc *p = outVexList[nextVex].firstArc; p != nullptr; p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc){
            int endID = p->outVexID == nextVex ? p->inVexID : p->outVexID;
            if(outVexList[endID].visited == false){
                awaitVexList.push_back(endID);
                awaitArcList.push_back(p);
                if(type == UDG && GetIdOf(p->gArc->edVex()) != endID)
                    p->gArc->reverseDirection();
            }
        }
        if(nextArc && !outVexList[nextVex].visited)
            nextArc->visit();
        outVexList[nextVex].visit();
    }
    if(generateForest){
        for(int i = 0; i < outVexList.size(); i++){
            if(outVexList[i].visited)   continue;
            awaitVexList.clear();
            awaitArcList.clear();
            awaitVexList.push_back(i);
            while(awaitVexList.size() > 0){
                int nextVex = awaitVexList.back();
                AMLArc *nextArc = awaitArcList.size() > 0 ? awaitArcList.back() : nullptr;
                awaitVexList.pop_back();
                if(nextArc)
                    awaitArcList.pop_back();
                for(AMLArc *p = outVexList[nextVex].firstArc; p != nullptr; p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc){
                    int endID = p->outVexID == nextVex ? p->inVexID : p->outVexID;
                    if(outVexList[endID].visited == false){
                        awaitVexList.push_back(endID);
                        awaitArcList.push_back(p);
                        if(type == UDG && GetIdOf(p->gArc->edVex()) != endID)
                            p->gArc->reverseDirection();
                    }
                }
                if(nextArc && !outVexList[nextVex].visited)
                    nextArc->visit();
                outVexList[nextVex].visit();
            }
        }
    }
}

void AMLGraph::BFS(int strtID, bool generateForest){
    if(strtID == -1)
        return;
    vector<int> awaitVexList;
    vector<AMLArc*> awaitArcList;
    awaitVexList.push_back(strtID);
    while(awaitVexList.size() > 0){
        int nextVex = awaitVexList[0];
        AMLArc *nextArc = awaitArcList.size() > 0 ? awaitArcList[0] : nullptr;
        awaitVexList.erase(awaitVexList.begin());
        if(nextArc)
            awaitArcList.erase(awaitArcList.begin());
        for(AMLArc *p = outVexList[nextVex].firstArc; p != nullptr; p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc){
            int endID = p->outVexID == nextVex ? p->inVexID : p->outVexID;
            if(outVexList[endID].visited == false){
                awaitVexList.push_back(endID);
                awaitArcList.push_back(p);
                if(type == UDG && GetIdOf(p->gArc->edVex()) != endID)
                    p->gArc->reverseDirection();
            }
        }
        if(nextArc && !outVexList[nextVex].visited)
            nextArc->visit();
        outVexList[nextVex].visit();
    }
    if(generateForest){
        for(int i = 0; i < outVexList.size(); i++){
            if(outVexList[i].visited)   continue;
            awaitVexList.clear();
            awaitArcList.clear();
            awaitVexList.push_back(i);
            while(awaitVexList.size() > 0){
                int nextVex = awaitVexList[0];
                AMLArc *nextArc = awaitArcList.size() > 0 ? awaitArcList[0] : nullptr;
                awaitVexList.erase(awaitVexList.begin());
                if(nextArc)
                    awaitArcList.erase(awaitArcList.begin());
                for(AMLArc *p = outVexList[nextVex].firstArc; p != nullptr; p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc){
                    int endID = p->outVexID == nextVex ? p->inVexID : p->outVexID;
                    if(outVexList[endID].visited == false){
                        awaitVexList.push_back(endID);
                        awaitArcList.push_back(p);
                        if(type == UDG && GetIdOf(p->gArc->edVex()) != endID)
                            p->gArc->reverseDirection();
                    }
                }
                if(nextArc && !outVexList[nextVex].visited)
                    nextArc->visit();
                outVexList[nextVex].visit();
            }
        }
    }
}

void AMLGraph::Dijkstra(int strtID){
    //Clear previous result
    ClearVisit();
    ResetDistance();
    //Set start vex info to all vertexes
    for(int i = 0; i < outVexList.size(); i++)
        outVexList[i].info->strtVexInfo = outVexList[strtID].info;
    //Start dijkstra
    outVexList[strtID].info->distance = 0;
    outVexList[strtID].access("strt");
    while(true){
        //Find next
        int minVexID = -1;
        for(int i = 0; i < outVexList.size(); i++){
            if(outVexList[i].visited || outVexList[i].info->distance == VexInfo::INF)
                continue;
            if(minVexID == -1)
                minVexID = i;
            else if(outVexList[i].info->distance < outVexList[minVexID].info->distance)
                minVexID = i;
        }
        if(minVexID == -1)
            break;
        //Set visit to edge and vex
        AMLArc *edge = FindArc(outVexList[minVexID].info->preVexID, minVexID);
        if(edge){
            if(type == UDG && GetIdOf(edge->gArc->edVex()) != minVexID)
                edge->gArc->reverseDirection();
            edge->visit();
        }
        outVexList[minVexID].visit();
        //Find adjacent
        for(AMLArc *p = outVexList[minVexID].firstArc; p != nullptr; p = p->outVexID == strtID ? p->nextOutArc : p->nextInArc){
            int endID = p->outVexID == minVexID ? p->inVexID : p->outVexID;
            if(!outVexList[endID].visited){
                if(GetIdOf(p->gArc->edVex()) != endID)
                    p->gArc->reverseDirection();
                p->access();
                if(outVexList[endID].info->distance == VexInfo::INF ||
                        outVexList[endID].info->distance > outVexList[minVexID].info->distance + p->weight){
                    outVexList[endID].info->preVexID = minVexID;
                    outVexList[endID].info->distance = outVexList[minVexID].info->distance + p->weight;
                    outVexList[endID].access(QString::asprintf("%d", outVexList[endID].info->distance));
                }
            }
        }
    }
}

ALGraph* AMLGraph::ConvertToAL(){
    ALGraph *converted = new ALGraph(type);
    for(int i = 0; i < outVexList.size(); i++)
        converted->AddVex(outVexList[i].info);
    for(int i = 0; i < outVexList.size(); i++){
        AMLArc *p = outVexList[i].firstArc;
        while(p != nullptr){
            if(p->outVexID == i)
                converted->AddArc(p->gArc);
            p = p->outVexID == i ? p->nextOutArc : p->nextInArc;
        }
    }
    return converted;
}
