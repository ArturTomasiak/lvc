#include <app.hpp>

bool lvc::help(int argc, char* argv[]) {
    std::cout << R"(Usage: lvc <command> [arguments]

lvc create
      Start the interactive setup in the current directory. Follow the prompts
      to create a repository from scratch or clone an existing repository.

lvc version ...
      Use wildcards, root-relative paths, exclusion rules and or incomplete paths,
      then approve the list of selected paths to record changes or cancel the operation.
      Required Arguments:
         desc "string"
      Example: lvc version / ~*.launch desc "UI restart machine notification on error handler and hard fault"
      Explanation: / selects all paths and ~*.launch unselects all .launch files
      Example: lvc version -desc "clearing display at initialization" main.c
      Explanation: will work despite main.c not being in the root folder

lvc checkout
      List files that have changed since the last version.

lvc diff ...
      Show what changed at the selected paths since the last version.
      Example: lvc diff src/main.cpp

lvc history
      Show the IDs, descriptions and which workspace does the version
      come from.
      Optional Arguments:
         workspace <string>
            specify a workspace (default current)
         depth <num>
            versions added as a result of a merge are treated as nested
            versions, where depth limits the max displayed nesting (default 1)
         length <num>
            the limit of how many versions are printed in total (default 20)
         all
            print every version

lvc revert <version-id> ...
      Restore the selected paths to their state in that version. If no paths were
      selected, the entire working directory is reverted, except ignored paths.
      Example: lvc revert threadx.c

lvc workspace <name>
      Create a workspace based on the current one.
      Optional Arguments:
         clean
            Creates an empty workspace

lvc goto <workspace>
      Switch to the named workspace and load its files.
      Example: lvc goto feature

lvc current
      Print the name of the current workspace.

lvc default <workspace>
      Set the repository's default workspace.

lvc category <workspace> <category>
      Assign a workspace to a category.
      Example: lvc category feature development

lvc list
      List workspaces grouped by category.

lvc merge <source-workspace>
      Merge the source workspace into the current workspace. If the source
      is in another repository, provide its path as the second argument.
      Optional Arguments
         second argument
            A second argument is treated as the path to the repository
            containing the source workspace, allowing for merging with
            a workspace from a different repository.
)";
    return true;
}