#include "InputHandler.hpp"

InputHandler::InputHandler(GLFWwindow* window) 
	: m_window{ window }, m_activeKeys {}, m_lastActiveKeys{}, m_activeButtons{}, m_lastActiveButtons{}, m_cursorPosition{}, m_lastCursorPosition{}, m_scrollDirection{}, m_lastScrollDirection{}
{
	m_activeKeys.resize(GLFW_KEY_COUNT);
	m_lastActiveKeys.resize(GLFW_KEY_COUNT);

	m_activeButtons.resize(GLFW_BUTTON_COUNT);
	m_lastActiveButtons.resize(GLFW_BUTTON_COUNT);



	glfwSetWindowUserPointer(m_window, this);

	auto key_func = [](GLFWwindow* window, int key, int scancode, int action, int mods) { static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods); };
	glfwSetKeyCallback(m_window, key_func);

	auto position_func = [](GLFWwindow* window, double x, double y) { static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->position_callback(window, x, y); };
	glfwSetCursorPosCallback(m_window, position_func);

	auto button_func = [](GLFWwindow* window, int button, int action, int mods) { static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->button_callback(window, button, action, mods); };
	glfwSetMouseButtonCallback(m_window, button_func);

	auto scroll_func = [](GLFWwindow* window, double x, double y) { static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->scroll_callback(window, x, y); };
	glfwSetScrollCallback(m_window, scroll_func);
}

const std::vector<bool>& InputHandler::active_keys() const
{
	return m_lastActiveKeys;
}
const std::vector<bool>& InputHandler::active_buttons() const
{
	return m_activeButtons;
}
const glm::vec2& InputHandler::cursor_position() const
{
	return m_cursorPosition;
}
const glm::vec2& InputHandler::scroll_direction() const
{
	return m_scrollDirection;
}

bool InputHandler::is_key_pressed(int key) const
{
	return m_activeKeys[key];
}
bool InputHandler::is_key_pressed_once(int key) const
{
	return m_activeKeys[key] && !m_lastActiveKeys[key];
}

bool InputHandler::is_moving_cursor_x() const
{
	return (m_cursorPosition - m_lastCursorPosition).x > 0;
}
bool InputHandler::is_moving_cursor_y() const
{
	return (m_cursorPosition - m_lastCursorPosition).y > 0;
}

bool InputHandler::is_button_pressed(int button) const
{
	return m_activeButtons[button];
}
bool InputHandler::is_button_pressed_once(int button) const
{
	return m_activeButtons[button] && !m_lastActiveButtons[button];
}

bool InputHandler::is_scrolling_x(int direction) const
{
	return m_scrollDirection.x == direction;
}
bool InputHandler::is_scrolling_y(int direction) const
{
	return m_scrollDirection.y == direction;
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (mods) return;

	if (action == GLFW_PRESS) m_activeKeys[key] = true;
	if (action == GLFW_RELEASE) m_activeKeys[key] = false;
}
void InputHandler::position_callback(GLFWwindow* window, double x, double y)
{
	m_lastCursorPosition = m_cursorPosition;
	m_cursorPosition = glm::vec2(x, y);
}
void InputHandler::button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (mods) return;

	if (action == GLFW_PRESS) m_activeButtons[button] = true;
	if (action == GLFW_RELEASE) m_activeButtons[button] = false;
}
void InputHandler::scroll_callback(GLFWwindow* window, double x, double y)
{
	m_lastScrollDirection = m_scrollDirection;
	m_scrollDirection = glm::vec2(x, y);
}

void InputHandler::update()
{
	m_lastActiveKeys = m_activeKeys;
	m_lastActiveButtons = m_activeButtons;

	m_lastScrollDirection = m_scrollDirection;
	m_scrollDirection = glm::vec2(0);

	glfwPollEvents();
}
