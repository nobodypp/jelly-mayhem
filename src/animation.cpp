#include "animation.hpp"


Animation::Animation(std::vector<sf::Texture>* frames, float fps)
    : fps(fps), 
      frames(frames), 
      timeFromRestart(sf::Time::Zero)
{}

void Animation::restart()
{
    timeFromRestart = sf::Time::Zero;
}

sf::Texture& Animation::getCurrentFrame() { return (*frames)[int(fps * timeFromRestart.asSeconds()) % frames->size()]; }

unsigned int Animation::getCurrentCycle() { return int(fps * timeFromRestart.asSeconds()) / frames->size(); }

unsigned int Animation::getCurrentFrameNumber() { return int (fps * timeFromRestart.asSeconds()) % frames->size();}

void Animation::update(sf::Time deltaTime) { timeFromRestart += deltaTime; }

void Animation::setFps(float fps) { this->fps = fps; }
