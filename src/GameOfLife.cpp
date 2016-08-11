#include <math.h>
#include <windows.h>
#include <GL/freeglut.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using namespace std;


typedef std::pair <long long, long long> coord;  // 8 byte signed long long to represent 64 bit signed coordinate //
vector<coord> displayObjects ;  // global displayObjects for Glut

///////////////////////////////////
// World representation by class //
////////////////////////////////////

class GameOfLife {
  private:
     std::map <coord, int> alive_cells;    // Alive cells
     std::map <coord, int> gestation_cells;// Neighbors likely to be alive, fate decided by count

     int iteration;  // Debug utility to indicate iteration count R-Pentomino stablize at 1103 iterations

     void parseFile(string file);
     void spreadseed();
     int isAliveCell(coord this_coord);
     int isGestationCell (coord this_coord);
     void markCellWithLiveNeighbor(coord this_coord);
     void spreadOrKill();

 public:
     void runIteration();
     void printAliveCells();
     void getAliveCells (vector<coord>&results);
     void initializeWorld( string filename );
}g;


void GameOfLife::initializeWorld(string file) {
    iteration = 0;

    gestation_cells.clear();
    alive_cells.clear();

    parseFile(file);
}

// Debug utility to print all alive cells //
void GameOfLife::printAliveCells() {
   for (std::map<coord,int>::iterator it=alive_cells.begin(); it!=alive_cells.end(); ++it) {
               cout<<"Cell alive"<<(it->first).first<<","<<(it->first).second<<endl;
    }
}

// Updates the result vector passed with coordinates of alive cells//
void GameOfLife::getAliveCells(vector<coord>&results) {

   for (std::map<coord,int>::iterator it=alive_cells.begin(); it!=alive_cells.end(); ++it) {
              results.push_back(make_pair((it->first).first,(it->first).second));
    }
}

// Gives current count if exists //
// Initial count -1 to indicate its not alive //
// 0 indicates reset state or that no neihbor is alive //
// any other number indicates that many alive neighbors //
int GameOfLife::isAliveCell (coord this_coord) {
    std::map<coord, int>::iterator it;
    it = alive_cells.find(this_coord);
    if(it == alive_cells.end()) {
         return -1;
    } else {
         return (it->second);
    }
}

// Gives current count if exist //
int GameOfLife::isGestationCell (coord this_coord) {
    std::map<coord, int>::iterator it;
    it = gestation_cells.find(this_coord);
    if(it == gestation_cells.end()) {
         return 0;
    } else {
         return (it->second);
    }
}

// Mark cell count to alive list or gestation list //

void GameOfLife::markCellWithLiveNeighbor (coord this_coord) {
     int live_count = isAliveCell(this_coord);

     if(live_count >= 0){
        live_count++;
        alive_cells[this_coord] = live_count;

     } else  {
         int gestation_count  = isGestationCell(this_coord);
         gestation_count++;
         gestation_cells[this_coord] = gestation_count;
     }
}


////////////////////////////////////////////////////////////
// Spread or kill implements the rules of the game //
//If an "alive" cell had less than 2 or more than 3 alive neighbors it becomes dead.//
//If a "dead" cell had *exactly* 3 alive neighbors, it becomes alive.//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameOfLife::spreadOrKill() {
    std::map<coord,int>::iterator it;
    it = alive_cells.begin();
    while(it!= alive_cells.end()){
         int live_count = it->second;
         if(live_count<2 || live_count > 3) {
            alive_cells.erase(it++);
         } else {
            it++;
         }
    }

    // Gestation cells are the neighbors of alive cells waiting for count to decide their fate //

    for (std::map<coord,int>::iterator it=gestation_cells.begin(); it!=gestation_cells.end(); ++it) {
         int live_count = it->second;
         coord gestation_coord = it->first;
         if(live_count == 3) {
             alive_cells[gestation_coord] = live_count;
         }
     }

    // We just need to remember the alive cells and update counts later based on new neighbor list //
    gestation_cells.clear();
    for (std::map<coord,int>::iterator it=alive_cells.begin(); it!=alive_cells.end(); ++it) {
           alive_cells[it->first]=0;
    }

}

///////////////////////////////////////////
//  Parse the input file                 //
//  If wrong format that line is ignored //

void GameOfLife::parseFile(string file){
    ifstream infile(file.c_str());
    if(!infile) {
        cout<< "File doesn't exist"<<endl;
        exit(1);
    }
    string raw;
    while(std::getline(infile, raw)) {
        string line;
        for (unsigned int i=0; i<raw.size(); ++i){
            if (raw[i]=='(' || raw[i]== ')' || raw[i] == ',' ) {
                continue;
            } else {
                line += raw[i];
            }
        }
        istringstream iss(line);
        long long coord[2];
        int count = 0;
        do
        {
           string sub;
           iss >> sub;
           if(sub.empty()) {
              continue;
            }
            coord[count++] = atoll(sub.c_str());
         } while (iss);
         if(count!=2) {
            cout<<"Incorrect input format"<<endl;
            continue;
         } else {
            alive_cells[make_pair(coord[0],coord[1])]  = 0;
         if(count!=2) {
               cout<<"Incorrect input format"<<endl;
               continue;
           }

         }
    }
    infile.close();
}

// Iteration runs 2 stages //
// Step1: Increment the count of each neighbor cell ( alive or empty) //
//  If neihbor is alive then its count is increased
//  if it is not alive then it is added to gestation list - likely to be alive if count reaches 3 //
// Step2: Based on count second step kills or bring to live cells //

void GameOfLife::runIteration () {

       for (std::map<coord,int>::iterator it=alive_cells.begin(); it!=alive_cells.end(); ++it) {
               long long x = (it->first).first;
               long long y = (it->first).second;

               // Mark all neighbors //
               //long long range = pow(2, 63) -1;
               //cout <<range;
               // Not handling x+1 crossing long long range as it will introduce wrapped around world Adding 1 will to max range will get to min range
               // Where last col neighbor is first column and vice versa //

               markCellWithLiveNeighbor(make_pair(x+1,y));
               markCellWithLiveNeighbor(make_pair(x+1,y+1));
               markCellWithLiveNeighbor(make_pair(x+1,y-1));
               markCellWithLiveNeighbor(make_pair(x,y+1));
               markCellWithLiveNeighbor(make_pair(x,y-1));
               markCellWithLiveNeighbor(make_pair(x-1,y));
               markCellWithLiveNeighbor(make_pair(x-1,y+1));
               markCellWithLiveNeighbor(make_pair(x-1,y-1));

        }
        spreadOrKill();
        iteration++;
        //cout<<"Iteration "<<iteration<<endl;
        //printAliveCells();

}


/////View dimension ///
long long min_x = -100;
long long max_x = 100;
long long min_y = -100;
long long max_y = 100;

/////////////////////////////////////////////////////////////////////////
// Bind Key arrow up/down/left/right for pan and +/- for 2xzoom(in/out)//
/////////////////////////////////////////////////////////////////////////

void ascii_key(unsigned char key, int x, int y)
{

    switch (key)
    {
        case '+':
             min_y /= 2;
             max_y /= 2;
             min_x /= 2;
             max_x /= 2;
             break;
        case '-':
             min_y *= 2;
             max_y *= 2;
             min_x *= 2;
             max_x *= 2;
             break;
    }
    glutPostRedisplay();
}
void special_key(int key, int x, int y)
{

    switch (key)
    {
        case GLUT_KEY_UP:
             min_y += 100;
             max_y += 100;
             cout<<"in here"<<endl;
             break;

        case GLUT_KEY_DOWN:
             min_y -= 100;
             max_y -= 100;
             break;

        case GLUT_KEY_LEFT:
             min_x -= 100;
             max_x -= 100;
             break;

        case GLUT_KEY_RIGHT:
             min_x += 100;
             max_x += 100;
             break;

    }
    glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////////////////
// Runs one iteration of Conway game per idle cycle and update display results //
/////////////////////////////////////////////////////////////////////////////////

void idle(void)
{
    g.runIteration();
    displayObjects.clear();
    g.getAliveCells(displayObjects);
    glutPostRedisplay();

    Sleep(50);
}

///////////////////////////////////////////////////////////
// Display the alive cells results produced by iteration //
///////////////////////////////////////////////////////////

void display() {
   static bool count_iter =0;
   count_iter = !count_iter;

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluOrtho2D(min_x,max_x,min_y,max_y);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();


   glBegin(GL_QUADS);

      // Flame color orange yellow alternate //
      if(count_iter) {
         glColor3f(1.0f, 0.5f, 0.0f);
      } else {
         glColor3f(1.0f, 1.0f, 0.0f);

      }


      for (std::vector<coord>::iterator it=displayObjects.begin(); it!=displayObjects.end(); ++it) {
            long long x = it->first;
            long long y = it->second;
            glVertex2f(x,y);
            glVertex2f(x+1,y);
            glVertex2f(x+1,y+1);
            glVertex2f(x,y+1);

       }

       displayObjects.clear(); // Clear the vector //

    glEnd();
    glFlush();  // Render now
    glutPostRedisplay();
}


int main(int argc, char** argv) {

   // Default input is "input.txt" located in exe directory else provide input from commandline //
   string filename("tests/Input.txt");
   if (argc == 2) {
        filename = argv[1];
   }

   glutInit(&argc, argv);                 // Initialize GLUT
   glutCreateWindow("ConeWay Life"); // Create a window with the given title
   glutDisplayFunc(display); // Register display callback handler for window re-paint
   glutIdleFunc(idle);

   glutKeyboardFunc(ascii_key);
   glutSpecialFunc (special_key);
   int window_width = glutGet(GLUT_SCREEN_WIDTH);
   int window_height = glutGet(GLUT_SCREEN_HEIGHT);

   int x_offset = 100;
   int y_offset = 100;

   int window_size_width = window_width - x_offset;
   int window_size_height = window_height - y_offset;

   glutReshapeWindow(window_size_width, window_size_height);
   glutPositionWindow(x_offset/2,y_offset/2); // Position the window's initial top-left corner

   g.initializeWorld(filename);


   glutMainLoop();           // Enter the event-processing loop
   return 0;

}


