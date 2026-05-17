#include<stdio.h>
#include<termios.h>
#include<time.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<errno.h>
#include<dirent.h>
#include<string.h>
#define mapwidth 20
#define mapheight 10
#define mapsize 30
#define s 6
#define g 7
//エラーコード
#define NO_DATA 100
#define INCORRECT_PASS 200
#define NORMAL_TERMINATION 777
#define NANKA_WAKANNAI 666
#define GACHIDE_WAKANNAI 999


//画面リフレッシュ
void clear() {
    printf("\033[2J\033[1;1H");
}
//表示
void MapPrint(int map[mapsize][mapsize]){
    clear();
    for(int i=0;i<mapsize;i++){
        for(int j=0;j<mapsize;j++){
            if(map[i][j]==0){
                putchar(' ');
            }
            else if (map[i][j]==1){
                putchar('*');
            }
            else if(map[i][j]==2){
                putchar('@');
            }
            else if(map[i][j]==s){
                putchar('S');
            }
            else if(map[i][j]==g){
                putchar('G');
            }
            
        }
        printf("\n");
    }
}
//移動関数
void go(int x,int y,int map[mapsize][mapsize],int *px,int *py){
    if(map[y+*py][x+*px]==0){
        map[*py][*px]=0;
        map[y+*py][x+*px]=2;
        *px+=x;
        *py+=y;
    }
    
    else
        printf("移動できません");
}

//迷路作成アルゴリズム
//乱数行動
void RandWalk(int x[],int y[]){
    for(int i=3;i>0;i--){
        int r=rand()%(i+1);
        int temp;
        temp=x[r];
        x[r]=x[i];
        x[i]=temp;
        temp=y[r];
        y[r]=y[i];
        y[i]=temp;
    }
}
void mining(int x,int y,int map[mapsize][mapsize]){
    map[y][x]=0;
    int dx[4]={-1,1,0,0};
    int dy[4]={0,0,1,-1};
    RandWalk(dx,dy);
    for(int i=0;i<4;i++){
        int NextX=x+(dx[i]* 2);
        int NextY=y+(dy[i]* 2);
        if(NextX<=0 || NextX>=mapsize-1 || NextY<=0 || NextY>=mapsize-1)continue;
        if(map[NextY][NextX]==1){
            map[y+dy[i]][x+dx[i]]=0;
            mining(NextX,NextY,map);
        }
    }
}
//マップ生成
void MkMap(int map[mapsize][mapsize],int start,int goal){
    for(int i=0;i<mapsize;i++){
        for(int j=0;j<mapsize;j++){
            map[i][j]=1;
        }
    }
    mining(start,1,map);
    //枠生成
    for(int i=0;i<mapsize;i++){
        map[0][i]=1;
    }
    for(int i=0;i<mapsize;i++){
        map[mapsize-1][i]=1;
    }
    for(int i=0;i<mapsize;i++){
        map[i][0]=1;
    }
    for(int i=0;i<mapsize;i++){
        map[i][mapsize-1]=1;
    }
    map[0][start]=s;
    map[mapsize-1][goal]=g;
    int check=mapsize-2;
    while((map[check][goal]==1)){
        map[check][goal]=0;
        check--;
    }
    check=0;
    while(!(map[check][start]==0 && (map[check][start+1]==0 || map[check][start-1]))){
        check++;
        map[check][start]=0;
    }
    map[1][start]=2;
}

//データロード
int DataLoad(char name[],int map[mapsize][mapsize],int *px,int *py){
    int temp;
    FILE *fp;
    fp=fopen(name,"r");
    if(fp==NULL)
        return NO_DATA;
    else{
        printf("Now Loading...");
        for(int i=0;i<mapsize;i++){
            for(int j=0;j<mapsize;j++){
                map[i][j]=fgetc(fp);
            }
        }
        fclose(fp);
        for(int i=0;i<mapsize;i++){
            for(int j=0;j<mapsize;j++){
                if(map[i][j]==2){
                    *py=i;
                    *px=j;
                    break;
                }
            }
        }
        printf("\rLet's go!!\r");
        return NORMAL_TERMINATION;
        
    }
}
//データセーブ
int DataSave(char name[],int map[mapsize][mapsize]){
    FILE *fp;
    if((fp=fopen(name,"w"))==NULL)
        return NO_DATA;
    else{
        printf("セーブ中です。終了しないでください。\n");
        for(int i=0;i<mapsize;i++){
            for(int j=0;j<mapsize;j++){
                fputc(map[i][j],fp);
                //fprintf(fp,"%d",map[i][j]);
            }
        }
        fclose(fp);
        return NORMAL_TERMINATION;
    }
}
//ログイン的な
void Name(char a[],int map[mapsize][mapsize]){
    DIR *dir=opendir(".Data");
    if(dir==NULL){
        printf("再起動してください");
    }
    struct dirent *entry;
    int count=1;
    entry=readdir(dir);
    entry=readdir(dir);
    while((entry=readdir(dir))!=NULL){
        printf("Name%d:%s\n",count,entry->d_name);
        count++;
    }
    closedir(dir);
    printf("呼び出したいデータの番号を入力してください。新しいデータを作りたい場合は-1を入力してください");
    int enter;
    scanf("%d",&enter);
    if(enter==-1){
        int flag=1;
        while(flag){
            printf("名前を入力してください(20文字以内)：");
            char name[20]="";
            scanf("%s",name);
            printf("%sでよろしいですか？ yes->1,no->0",name);
            int temp;
            scanf("%d",&temp);
            if(temp==1){
                char path[30]=".Data/";
                strcat(path,name);
                FILE *fp;
                fp=fopen(path,"w");
                fclose(fp);
                strcat(a,path);
                srand(time(NULL));
                MkMap(map,(rand()%(mapsize-4))+2,(rand()%(mapsize-4))+2);
                DataSave(path,map);
                flag=0;
            }
        }
    }
    else if(0<enter && enter<count){
        DIR *dir=opendir(".Data");
        struct dirent *e;
        for(int i=1;i<enter+2;i++){
            e=readdir(dir);
        }
        e=readdir(dir);
        char name[20]="";
        strcat(name,e->d_name);
        char path[30]=".Data/";
        strcat(path,name);
        strcat(a,path);
        closedir(dir);
    }
    else{
        printf("無効な値です。");
    }
}   
//-----本体-------
int main(void){
    const char *dir=".Data";
    struct stat st;
    if(stat(dir,&st)==0){}
    else if(errno==ENOENT){
        if(mkdir(dir,0777)==0){}
        else{
            printf("セーブデータの作成に失敗しました。再度起動してください。");
            return 0;
        }
    }
    srand(time(NULL));
    int px=1;
    int py=1;
    int flag=1;
    int start=1;
    int goal=0;
    char input;
    int map[mapsize][mapsize];
    char name[30];
    Name(name,map);
    //ここらへんに迷路生成アルゴリズム
    start=(rand()%(mapsize-4))+2;
    goal=(rand()%(mapsize-4))+2;
    MkMap(map,start,goal);
    px=start;
    py=1;
    map[1][start]=2;
    DataLoad(name,map,&px,&py);
    MapPrint(map);
    switch (DataSave(name,map))
    {
        case NORMAL_TERMINATION:break;
        case NO_DATA:printf("セーブできませんでした。終了します。\n");return 0;
    }
    while(flag){
        input=getchar();
        int valiable_input=0;
            switch (input){
                case 'w':go(0,-1,map,&px,&py);break;
                case 'a':go(-1,0,map,&px,&py);break;
                case 's':go(0,1,map,&px,&py);break;
                case 'd':go(1,0,map,&px,&py);break;
                case 'p':DataSave(name,map);return 0;
                default:printf("無効な入力");clear();break;
            }
        if(map[py+1][px]==g){
            switch (DataSave(name,map))
            {
            case NORMAL_TERMINATION:break;
            case NO_DATA:printf("セーブできませんでした。終了します。");return 0;
            }
            start=goal;
            goal=(rand()%(mapsize-4))+2;
            MkMap(map,start,goal);
            px=start;
            py=1;
            map[py][px]=2;
            MapPrint(map);
            continue;
        }
        clear();
        MapPrint(map);
    }
    return 0;
}