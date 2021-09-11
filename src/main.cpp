#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


#include <cmath>
#include <iostream>
#include <chrono>

/// Window settings
#define HEIGHT 512
#define WIDTH 512
#define WINDOW_NAME "gameWindow"

// Game settings
#define WALL_THICKNESS 20
#define ups 1000/60		/*millis per update*/
#define GRAVITY 0.05	/*Movement pixels force per second: Not implemented yet as a static g/s*/

/// <summary>
/// A tuple which contians 4 variables. Mainly used as a rectangle in the update to define walls
/// </summary>
struct Rectangle {
	unsigned int x, y, width, height;
};


/// <summary>
/// Defines the bouncing ball. A better implementations can be done but since it is a bonus assignment I rather not spend more time polishing
/// </summary>
struct Ball {
	double x; // y should be double only if a more physical gravititional force with air resistance will be applied
	unsigned int y;
	unsigned int radius;
	unsigned char r, g, b;
	double xVelocity, yVelocity;
};


/// <summary>
/// Defines the Player.
/// </summary>
struct Racket {
	unsigned int x, y;
	unsigned int width,height;
	unsigned char r, g, b;
	unsigned int speed; // Movement speed per ups
};

/// <summary>
/// Defines the Ball and the player states and whether the game is still running or not
/// </summary>
struct GameState {
	Ball ball;
	Racket player;
	bool running;

	GameState( Ball b = { WIDTH / 2, 100, 10 , 0,255,255 ,0 , 0}, 
			   Racket p = { WIDTH / 2, HEIGHT - 80, 90, 15, 255, 0, 0, 10 })
		: ball(b), player(p), running(true)
	{}
};



/* Rectangle-Circle Collision algorithm from: https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection*/
bool hit(const Ball& ball, const Rectangle& rect)
{
	unsigned int cdx = std::abs((long)((unsigned int)ball.x - (rect.x + rect.width / 2)));
	unsigned int cdy = std::abs((long)(ball.y - (rect.y + rect.height / 2)));

	if (cdx > (rect.width / 2 + ball.radius)) { return false; }
	if (cdy > (rect.height / 2 + ball.radius)) { return false; }

	if (cdx <= (rect.width / 2)) { return true; }
	if (cdy <= (rect.height / 2)) { return true; }

	unsigned int cornerDistance_sq = (cdx - rect.width / 2) ^ 2 + (cdy - rect.height / 2) ^ 2;

	return (cornerDistance_sq <= (ball.radius ^ 2));
}


/// <summary>
/// Called in the game loop. It defines the logic of the game (Handles input and define collisions and other stuff)
/// </summary>
/// <param name="gs">A game state struct in which the ball and the racket (player) are defined</param>
void update(GameState& gs) {
	int key = cv::pollKey();

	if (key == 27) { // Exiting the game
		gs.running = false;
		return;
	}

	// Player movement
	switch (key) {
	case 'a':
		// Adding player-border collision and movement
		gs.player.x = (gs.player.x >= WALL_THICKNESS + gs.player.speed) ? gs.player.x - gs.player.speed : WALL_THICKNESS;
		break;
	case 'd':
		// Adding player-border collision and movement
		gs.player.x = (gs.player.x <= (WIDTH - WALL_THICKNESS - gs.player.width - gs.player.speed)  ) ? gs.player.x + gs.player.speed : (WIDTH - WALL_THICKNESS - gs.player.width);
		break;
	default:
		break;
	}// Player movement


	Rectangle l = { 0, 0 ,WALL_THICKNESS, HEIGHT }; // left wall
	Rectangle r = { WIDTH - WALL_THICKNESS , 0 ,WALL_THICKNESS , HEIGHT }; // right wall
	Rectangle t = { 0, 0 , WIDTH, WALL_THICKNESS }; // top wall


	if (hit(gs.ball, l)) {// Left wall collision
		gs.ball.xVelocity = -gs.ball.xVelocity;
		gs.ball.x = (double)(l.x + l.width + gs.ball.radius) + gs.ball.xVelocity + 0.5; // adding bias 
	}// Left wall collision
	else if (hit(gs.ball, r)) { // right wall hit
		gs.ball.xVelocity = -gs.ball.xVelocity;
		gs.ball.x = double(r.x - gs.ball.radius) + gs.ball.xVelocity - 0.5;
	} // right wall hit
	else if (hit(gs.ball, t)) { // top wall hit
		gs.ball.yVelocity = -(gs.ball.yVelocity - 19 * GRAVITY); // BUG: Needs to be dynamic instead of Static
		gs.ball.y = t.height + gs.ball.radius + gs.ball.yVelocity;
	} // top wall hit
	else if (hit(gs.ball, { gs.player.x , gs.player.y, gs.player.width, gs.player.height })) { // ball-player hit
		gs.ball.yVelocity = -(gs.ball.yVelocity - 19 * GRAVITY); // BUG: Needs to be dynamic instead of Static
		gs.ball.y = gs.player.y - gs.ball.radius + gs.ball.yVelocity;
		
		int den = (unsigned int)gs.ball.x - (gs.player.x + gs.player.width / 2);
		if (den != 0) {
			double refl = (double)den / (double) (gs.player.width / 2); // den = +-1, +-2, +-3, +-4....., radius = 1,2,3,4,5...
			gs.ball.xVelocity = (std::abs(gs.ball.xVelocity + refl * 2) > 2)? gs.ball.xVelocity : gs.ball.xVelocity + refl * 2;
		}
	}
	else { // No hits (floating ball)
		/*If we are falling then increase velocity, otherwise decrease velocity by gravity*/
		gs.ball.yVelocity += (gs.ball.yVelocity + GRAVITY > gs.ball.yVelocity) ? GRAVITY : -GRAVITY; // Acceleration applied
		gs.ball.y += gs.ball.yVelocity;	// Move the ball
		gs.ball.x += gs.ball.xVelocity; // Movoes the ball on the X_axis
		if (gs.ball.y >= HEIGHT) {
			gs.running = false;
			return;
		}
	}

}


/// <summary>
/// Applies rendering of the game states to the image buffer
/// </summary>
/// <param name="img">An open cv Matrix image</param>
/// <param name="gs">GameState struct in which the game entities positions and settings are defined in</param>
void render(cv::Mat& img, const GameState& gs) {
	img = cv::Mat::zeros(img.size(), img.type());				// Clearing the Image

	/* Not real representation of Rectangles. It is more of a 4 size tuple.*/
	Rectangle l = { 0, 0 ,WALL_THICKNESS, HEIGHT };
	Rectangle t = { 0, 0 , WIDTH, WALL_THICKNESS };
	Rectangle r = { WIDTH - WALL_THICKNESS , 0 , WIDTH, HEIGHT };

	/* Rendering the Rectangles */
	cv::rectangle(img, cv::Point(l.x, l.y), cv::Point(l.width, l.height), cv::Scalar(190, 180, 180), cv::FILLED);	// Left border
	cv::rectangle(img, cv::Point(t.x, t.y), cv::Point(t.width, t.height), cv::Scalar(190, 180, 180), cv::FILLED);	// Top border
	cv::rectangle(img, cv::Point(r.x, r.y), cv::Point(r.width, r.height), cv::Scalar(190, 180, 180), cv::FILLED); // Right border
	cv::circle(img, cv::Point(gs.ball.x,gs.ball.y), gs.ball.radius , cv::Scalar(gs.ball.r, gs.ball.g, gs.ball.b), cv::FILLED); // Drawing the ball
	cv::rectangle(img, cv::Point(gs.player.x, gs.player.y),
					   cv::Point(gs.player.x+gs.player.width, gs.player.y + gs.player.height),
					   cv::Scalar(gs.player.r, gs.player.g, gs.player.b), cv::FILLED);
	cv::imshow(WINDOW_NAME, img);
}



/// _____________________________________ MAIN FUNCTION BELOW _____________________________________________


int main() {	
	// Game variables Initializing
	GameState gs = GameState();
	cv::Mat img = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now(); // Time variable


	// Window Initializing
	cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);
	//cv::setMouseCallback(WINDOW_NAME, mouseHandler, &gs.running);


	while (gs.running) {	// Game Loop
		std::chrono::high_resolution_clock::time_point currentTIme = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> timeSpan = (std::chrono::duration<double>)(currentTIme - lastTime);

		if (timeSpan.count() * 1000 > ups) {
			lastTime = currentTIme;
			update(gs);
			render(img, gs);
		}
	}// Game Loop

	

	while (true) {// Game over rendering
		int key = cv::waitKey(10);
		if (key > 0) break;
		img = cv::Mat::zeros(img.size(), img.type());
		cv::rectangle(img , cv::Point(0,0), cv::Point(WIDTH, HEIGHT), cv::Scalar(0,0,0) ,cv::FILLED );
		cv::putText(img, "Game Over!", cv::Point(WIDTH/3 ,HEIGHT/2), cv::FONT_ITALIC, 1, cv::Scalar(255,255,255), 3, 8);
		cv::putText(img, "press any key to exit", cv::Point(WIDTH / 4, HEIGHT / 1.7), cv::FONT_ITALIC, 0.8, cv::Scalar(255, 255, 255), 1, 8);
		cv::imshow(WINDOW_NAME,img);
	}// Game over rendering

	std::cout << "Bye!" << std::endl;
	return 0;
}