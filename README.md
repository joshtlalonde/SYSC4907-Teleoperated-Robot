# SYSC4907-Teleoperated-Robot
Capstone Project to create a teleoperated robot with bilateral haptic feedback 

# Github Rule of Thumb
**DO NOT** commit to main branch.
Create a new branch and commit to that branch, then submit a pull-request. 
The pull-request should be reviewed by someone else before pushing.

## Getting Started on Github
Githubs quickstart guide is a good place to start: https://docs.github.com/en/get-started/quickstart.
You will need to setup `git` using this guide: https://docs.github.com/en/get-started/quickstart/set-up-git#setting-up-git

### Cloning
Cloning is how you download the repo onto your computer to begin working on the code.

  - You will need to create a "Personal Access Token"
    - Go to https://github.com/settings/tokens
    - Select "Generate New Token"
    - Choose the "Classic" version
    - Enter a note and check all of the boxes
    - Copy the key and save somewhere for safe keeping
  - Now with the key we can clone the repo
    - Go to the repo home page: https://github.com/joshtlalonde/SYSC4907-Teleoperated-Robot/tree/main (link may not work)
    - Select the green "Code" drop down menu
    - Copy the https url
  - Now to download using git clone
    - Open the terminal and cd into a directory you would like to download the repo to
    - Run the following command: `git clone https://<GHusername>:<Token>github.com/<GHusername>/SYSC4907-Teleoperated-Robot.git`
      - GHusername: is your github username
      - Token: is the token we previously generated
  - This should download the git repo into the current directory

### Creating a branch
Branch is to separate the code you are working on from the code that has been established in the main branch. The purpose of branching is to ensure changes made by yourself in one file do not effect another persons work if they are working on the same file.

- Use the command `git checkout -b <branchname>` to create and move to a new branch

### Merging a change
Merging changes is how you upload your local changes to github.
- Add the files that you have changed
  - `git add <file1> <file2>`
- Commit the changes
  - `git commit`
  - A prompt will appear asking you to add a message
  - Type "i" to insert text into the prompt
  - When done press "escape" and tpye ":wq" to save and quit
- Merge and push the commit
  - `git push -u origin <branch>`
 
### Fetching changes
Fetching or Pulling is used to redownload code from github onto your local machine.
- To fetch/pull code from github you must specify the branch
  - `git pull origin <branch>`
  - If you want to pull from the main branch use "main"
 
# Repository Structure
- First level directory contains folders for each functional unit in the system
  - Arm: Contains code for controlling the robot arm
  - Calculations: Contains code for calculating the kinematic equations based on input from robot arm and virtual environment
  - Environment: Contains code for the virtual environment
