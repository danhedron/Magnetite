#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"
#include <mutex>
#include <deque>
#include <functional>

class Player;
class Renderer;
class TextureManager;
class InputManager;
class World;
class Character;
class BaseGame;
class ResourceManager;
class ScriptWrapper;

/** @class MagnetiteCore
 * Responsible for managing stuff
 */
class MagnetiteCore
{
protected:

	/**
	 * Engine Variables
	 */
	sf::RenderWindow	mWindow;
	sf::Clock	mClock;
	Renderer*	mRenderer;
	ResourceManager*	mResourceManager;
	ScriptWrapper*	mScriptWrapper;
	TextureManager* 	mTextureManager;
	InputManager*	mInputManager;
	World*		mWorld;
	bool		mContinue;
	float		mTimescale;

	/**
	 * Physics
	 */
	btBroadphaseInterface* mPBroadphase;
	btDefaultCollisionConfiguration* mPCConfig;
	btCollisionDispatcher* mCCDispatch;
	btSequentialImpulseConstraintSolver* mSolver;
	btDiscreteDynamicsWorld* mPhysicsWorld;
	btCollisionShape* mGroundShape;
	btDefaultMotionState* mGroundState;
	btRigidBody*		mGroundBody;

	/**
	 * Game stuff 
	 */
	std::vector<Character*>	mCharacters;
	BaseGame*	mGame;
	
public:
	MagnetiteCore(void);
	~MagnetiteCore(void);

	/**
	 * Singleton instance of the MagnetiteCore
	 */
	static MagnetiteCore *Singleton;

	/**
	 * @return a pointer to the Texture Manager
	 */
	TextureManager *getTextureManager();

	/**
	 * Returns a pointer to the Resource Manager
	 */
	ResourceManager* getResourceManager();
	
	/**
	 * Returns a pointer to the script manager
	 */
	ScriptWrapper* getScriptManager();

	/**
	 * @return a pointer to the renderer
	 */
	Renderer* getRenderer();

	/**
	 * @return a pointer to the game object
	 */
	BaseGame* getGame();
	
	/**
	 * Starts a new game of the specified type
	 */
	void startGame( const std::string& type );

	/**
	 * Creates a new world
	 */
	void newWorld( std::string name );

	/**
	 * Loads an exsisting world
	*/
	void loadWorld( std::string name );

	/**
	 * Retuns a pointer to the world
	 */
	World* getWorld();

	/**
	 * retuns the physics world
	 */
	btDiscreteDynamicsWorld* getPhysicsWorld();
	
	/**
	 * Global Mutex for the Physics system.
	 */
	std::mutex physicsMutex;

	/**
	 * Unloads the currently loaded world (if any)
	 */
	void unloadWorld();

	/**
	 * Creates a character
	 */
	Character* createCharacter();

	/**
	 * Destroys a character
	 */
	void destroyCharacter( Character* c );
	
	/**
	 * Stores a player.
	 */
	void addPlayer( Player* p );

	/**
	 * Player Input - Movement
	 */
	void inputMovement( const Vector3 &v );

	/**
	 * Player Input - Mouse
	 */
	void mouseMoved( const float x, const float y );

	/** 
	 * Returns the timescale
	 */
	float getTimescale();

	/**
	 *
	 */
	void screenshot();

	float mLastX;
	float mLastY;

	/**
	 * Initalizes the opencraft engine,
	 * @param argc The programs' argument count
	 * @param argv the argument list
	 */
	void init(int argc, char **argv);

	/**
	 * Initalize the physics engine
	 * This is called automatically.
	 */
	void initalizePhysics();

	void go();

	void exit();
};

#endif
