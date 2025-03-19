# **Fishing Planet Auto Clicker**

## **Overview**
Fishing Planet Auto Clicker is a powerful utility designed to automate mouse clicks, specifically tailored for use with the Fishing Planet game on the Windows operating system. This tool enhances gameplay by automating repetitive mouse click actions, allowing players to focus on strategic aspects rather than manual clicking.

Fishing Planet Auto Clicker offers:
- **Stop & Go Mode:** Automates the pull and stop actions with adjustable durations, perfect for simulating the rhythm of using a lure in Fishing Planet.
- **Catch Mode:** Continuously holds the left mouse button to simulate holding the fishing rod.

Other modes are currently under development and will be available in future updates.

## **Key Features**
- **Stop & Go Mode:** Automates pull and stop actions with adjustable durations.
- **Catch Mode:** Continuously holds the left mouse button for specific actions.
- **Customizable Settings:** Modify base pull and stop durations through an intuitive graphical user interface (GUI).
- **Custom Key Bindings:** Assign specific keys to activate or deactivate modes, ensuring personalized control.
- **Registry Settings Storage:** Saves your configurations to the Windows registry, maintaining settings between sessions.
- **Status Indicator:** Displays the current state of the Auto Clicker (e.g., Active, Inactive).

## **Installation and Usage**

### **Prerequisites**
- **Operating System:** Windows 10 or later.
- **Development Environment:** Visual Studio Code (for compiling the source code).
- **Dependencies:** No external libraries are required beyond the standard Windows API.

### **Installation Steps**
1. **Clone the Repository:**
    ```sh
    git clone https://github.com/literataa/AutoClickerFishingPlanet.git
    ```
2. **Open the Project:**
    - Navigate to the cloned repository directory.
    - Open the solution file `main.cpp` in Visual Studio Code.

3. **Build the Project:**
    - Select your desired build configuration (Debug/Release).
    - Build the solution using `Terminal > Run Build Task` or by pressing `Ctrl + Shift + B`.

4. **Run the Application:**
    - After a successful build, run the executable.
    - Alternatively, just run the `.exe` file provided in the repository.

### **Usage Guide**
1. **Launching the Tool:**
    - Upon running the application, a hidden console window will minimize to the system tray.

2. **Configuring Settings:**
    - **Pull Duration:** Set the duration (in milliseconds) for the "Go" action.
    - **Stop Duration:** Set the duration (in milliseconds) for the "Stop" action.
    - **Assign Keys:**
        - **Stop & Go Key:** Click the "Stop & Go Key" button and press your desired key.
        - **Catch Key:** Click the "Catch Key" button and press your desired key.

3. **Saving Settings:**
    - After configuring, click the "Save" button to store your settings in the Windows registry.

4. **Activating Modes:**
    - **Stop & Go Mode:** Press the assigned Stop & Go key to toggle the mode on or off.
    - **Catch Mode:** Press the assigned Catch key to toggle the mode on or off.

5. **Monitoring Status:**
    - The status label will display the current state of the Auto Clicker (e.g., "Stop & Go Active," "Catch Active," or "Inactive").

## **Contributing**
We welcome contributions from the community to enhance the functionality and features of Auto Clicker Windows. Whether you're reporting bugs, suggesting improvements, or submitting code enhancements, your input is valuable.

1. **Fork the Repository:**
    - Click the "Fork" button at the top right of the repository page.

2. **Clone Your Fork:**
    ```sh
    git clone https://github.com/yourusername/AutoClickerFishingPlanet.git
    ```

3. **Create a Feature Branch:**
    ```sh
    git checkout -b feature/YourFeatureName
    ```

4. **Commit Your Changes:**
    ```sh
    git commit -m "Add Your Feature Description"
    ```

5. **Push to Your Fork:**
    ```sh
    git push origin feature/YourFeatureName
    ```

6. **Submit a Pull Request:**
    - Navigate to the original repository and submit a pull request from your feature branch.

## **License**
This project is licensed under the [MIT License](LICENSE). You are free to use, modify, and distribute this software provided you include the original license and copyright.

## **Disclaimer**
**Use this tool at your own risk.** The developers are not responsible for any consequences that may arise from using this tool, including but not limited to violations of game terms of service, account bans, or hardware/software issues.

---

For any further questions or support, feel free to open an issue in the repository or reach out to the maintainer.
