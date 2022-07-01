#include<iostream>
#include<map>
#include<set>
#include <sstream>
using namespace std;
//�����������ip��ַ�ʹ��������Ϣ 
struct nodeinfo{
	string IP;
	map<uint32_t, std::string> data; //key�����ݵ�hashֵ��value�����ݱ����value 
	
};
class ConsistentHash {
private:
    map<uint32_t, nodeinfo> serverNodes;
        // ����ڵ㣬key �ǹ�ϣֵ��value �ǻ����� ip ��ַ
   // set<string> physicalNodes;  // ��ʵ�����ڵ� ip
    //int virtualNodeNum;  // ÿ�������ڵ����������ڵ����
public:
    ConsistentHash() {
//        physicalNodes.insert(std::string("192.168.1.101"));
//        physicalNodes.insert(std::string("192.168.1.102"));
//        physicalNodes.insert(std::string("192.168.1.103"));
//        physicalNodes.insert(std::string("192.168.1.104"));
    };
    ~ConsistentHash() {
        serverNodes.clear();
    };
    static uint32_t FNVHash(string key);
    void Initialize();
    void AddNewPhysicalNode(const string& nodeIp);
    void DeletePhysicalNode(const string& nodeIp);
     auto  GetServerIndex(uint32_t partition);
    void StatisticPerf(string& label, int left, int right);
    void ClientAddData(const string& key,const string& value);
    void ClientDelData(const string& key);
    void ClientDisplayData(const string& nodeIp);
    void ClientFindData(const string& key);
};
// 32 λ�� Fowler-Noll-Vo ��ϣ�㷨
// https://en.wikipedia.org/wiki/Fowler�CNoll�CVo_hash_function
uint32_t ConsistentHash::FNVHash(string key) {
    const int p = 16777619;
    uint32_t hash = 2166136261;
    for (int idx = 0; idx < key.size(); ++idx) {
        hash = (hash ^ key[idx]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    if (hash < 0) {
        hash = -hash;
    }
    return hash;
}
auto ConsistentHash::GetServerIndex(uint32_t partition)//����Ѱ����Ӧ������ 
{
    //uint32_t partition = FNVHash(key);
    auto it = serverNodes.lower_bound(partition);
    // �ػ���˳ʱ���ҵ�һ�����ڵ��� partition ������ڵ�
    if(it == serverNodes.end()) {
        if (serverNodes.empty()) {
            std::cout << "no available nodes" << '\n';
        }
        return serverNodes.begin();
    }
    return it;
}

void ConsistentHash::Initialize()//��node���г�ʼ�� ,����ĸ�node�ڵ� 
{
	string ips[4]={"192.168.1.101","192.168.1.102","192.168.1.103","192.168.1.104"};
     cout << "==== " << "��ʼ�� ����4��node" << " ====" << '\n';
    for(int i=0;i<4;i++) {
        
            
            uint32_t partition = FNVHash(ips[i]);
            nodeinfo nodeinfomation;
			nodeinfomation.IP=ips[i];
			cout<<"IP:"<<ips[i]<<"  "<<"HASH: "<<partition<<endl;
			serverNodes.insert({partition, nodeinfomation});
        }
    
}
// ����һ������ڵ�ʱ�������Ѿ��洢������ 
void ConsistentHash::AddNewPhysicalNode(const string& nodeIp)
{
		uint32_t partition = FNVHash(nodeIp);
	 auto it = GetServerIndex(partition);
        
        nodeinfo nodeinfomation;
		nodeinfomation.IP=nodeIp;	
        serverNodes.insert({partition, nodeinfomation});
        if (!it->second.data.empty()) {//����ɾ���ڵ������ת�Ƶ���һ���ڵ� 
       	   for (auto i = it->second.data.begin(); i != it->second.data.end(); i++) {
       	   	auto serverNode = GetServerIndex(i->first);
	         serverNode->second.data.insert({i->first, i->second});//�������ݴ洢 
      
          	}
       }
        
    
}
void ConsistentHash::DeletePhysicalNode(const string& nodeIp)//ɾ������ڵ� 
{
    
        
        uint32_t partition = FNVHash(nodeIp);//ͨ��ip������Ӧӳ��Ĺ�ϣֵ 
        auto it = serverNodes.find(partition);//�ҵ��������ڻ��е�λ��
        //?���п����Ҳ����� 
        auto itnext=it; //�ҵ���һ���ڵ� 
          	itnext++;
        if(itnext==serverNodes.end()) {//��Ϊ�Ǹ��� 
        	itnext=serverNodes.begin();
		}
		
   
       if (!it->second.data.empty()) {//����ɾ���ڵ������ת�Ƶ���һ���ڵ� 
       	   for (auto i = it->second.data.begin(); i != it->second.data.end(); i++) {
            	itnext->second.data.insert({i->first, i->second});//�������ݴ洢 
          	}
       }
       //	cout<<"nextshuju��С"<<itnext->second.data.size()<<endl; 
        if(it != serverNodes.end()) {
            serverNodes.erase(it);
        }
        
        
    
}
void ConsistentHash::ClientAddData(const string& key,const string& value){
	uint32_t partition = FNVHash(key);
	auto serverNode = GetServerIndex(partition);
	serverNode->second.data.insert({partition, value});//�������ݴ洢 
	cout<<"key:"<<key<<" "<<"HASH:"<<partition <<"���룺"<<serverNode->second.IP<<endl;
	//cout<<"���ݴ�С"<<nodeinfomation.data.size()<<endl;
}
void ConsistentHash::ClientDelData(const string& key){//ͨ��keyֵɾ������ 
	uint32_t partition = FNVHash(key);
	auto serverNode = GetServerIndex(partition);
	serverNode->second.data.erase(partition);//ɾ������ 
}
void ConsistentHash::ClientFindData(const string& key){//ͨ��keyֵ�������� 
	uint32_t partition = FNVHash(key);
	auto serverNode = GetServerIndex(partition);
	auto it=serverNode->second.data.find(1);
	cout<<"���ݴ洢�ڵ�IP: "<< serverNode->second.IP<<endl;
}
void ConsistentHash::ClientDisplayData(const string& nodeIp){// չʾ�ڵ����� 
	uint32_t partition = FNVHash(nodeIp);
	auto it = GetServerIndex(partition);
	 if (!it->second.data.empty()) {
       	   for (auto i = it->second.data.begin(); i != it->second.data.end(); i++) {
            	cout<<"HASH: "<<i->first<<"  "<<"value: "<<i->second<<endl;
          	}
       }
	
}

void client(ConsistentHash ch){
	ch.Initialize();
        while(1)//
        {
		printf ( "\n" );
		printf ( "\n" );
		printf ( "            	******************ConsistentHash*************\n" );
		printf ( "	______________________________________________________________________________\n" );
		printf ( "	|---->                       ��ѡ��������Ҫ������ѡ��                    <----|\n" );
		printf ( "	|----> 1: ��������                                                         <----|\n" );
		printf ( "	|----> 2��ɾ������                                                <----|\n" );
		printf ( "	|----> 3��ɾ���ڵ�                                                          <----|\n" );
		printf ( "	|----> 4�������µĽڵ�                                                          <----|\n" );
		printf ( "	|----> 5���鿴�ڵ�����                                              <----|\n" );
		printf ( "	|----> 6����������                                             <----|\n" );
		printf ( "	|__________ ___________________________________________________________________|\n" );
		int choice;
		string s1,s2,s3,s4,s5;
		printf("���������ѡ��\n");
		scanf("%d",&choice);
		switch(choice)
		{
			 
			case 1:
				
			cin>>s1,cin>>s2;
			 	 ch.ClientAddData(s1,s2); 
			 
			break;
			case 2:
				cout<<"����ɾ�����ݵ�keyֵ"<<endl; 
				cin>>s1;
			 	 ch.ClientDelData(s1); 
			break;
			case 3:
				cout<<"����ɾ���ڵ��ip"<<endl; 
				cin>>s1;
			 	 ch.DeletePhysicalNode(s1);
			break;
			case 4:
				cout<<"�������ڵ��ip"<<endl; 
				cin>>s1;
			 	 ch.AddNewPhysicalNode(s1);
			break;
			case 5:
				cout<<"����鿴�ڵ��ip"<<endl; 
				cin>>s1;
			 	 ch.ClientDisplayData(s1);
			break;
			case 6:
				cout<<"����鿴�ڵ��keyֵ"<<endl; 
				cin>>s1;
			 	 ch.ClientFindData(s1);
			break;
				
        }
        
        }

} 
int main(){
	
	ConsistentHash ch;
    client(ch);
    
	return 0;
	
}
