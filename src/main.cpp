//#include <cstdlib>
#include <iostream>
#include <string>

#include "../include/bst.h"
#include "../include/word.h"
#include "../include/utility.h"


void print_menu();
int choice();
ds::bst<word>::Node * find_closest_node(ds::bst<word>::Node * node, const char & letter, const int index);
void print_suggestions_helper(
  const ds::bst<word>::Node * subtree,
  ds::list<std::string> * suggestion_list,
  std::string matching_substr);
void print_suggestions(word * search);

void add_word_helper(std::string * message);

void add_word();
void search_word();
void delete_word();
void update_word();
void print_tree();


static ds::bst<word> WORD_TREE;


int main() {
  util::load_database(&WORD_TREE);

  while(1) {
    print_menu();

    switch (choice()) {
      case 1:
        add_word();
        break;
      case 2:
        search_word();
        break;
      case 3:
        delete_word();
        break;
      case 4:
        update_word();
        break;
      case 5:
        print_tree();
        break;
      case 6:
        util::clear_screen();
        util::save_changes(&WORD_TREE);
        exit(0);
      default:
        std::cout << "Invalid input!" << std::endl;
        break;
    }
  }

  return 0;
}

//################################################//
//------------------------------------------------//

/*
 * Prints the main menu to view the operations
 *
 * Parameters
 * ----------
 * none
 *
 * Returns
 * -------
 * none
 */
void print_menu() {
  util::clear_screen();

  std::cout << "                                 Word Dictionary" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;
  std::cout << "1. Add Word" << std::endl;
  std::cout << "2. Search Word" << std::endl;
  std::cout << "3. Delete Word" << std::endl;
  std::cout << "4. Update Word" << std::endl;
  std::cout << "5. Display Word-Tree" << std::endl;
  std::cout << "6. Quit" << std::endl;
  std::cout << std::endl;
}

/*
 * Gets user choice of operation and
 * checks if the choice is correct or not,
 * after which it returns the choice
 *
 * Parameters
 * ----------
 * none
 *
 * Returns
 * -------
 * int
 *  choice in an integer value
 */
int choice() {
  int choice = {};

  do {
    std::cout << "Please enter a corresponding number: " << std::endl;
    std::cin >> choice;

    if (std::cin.fail() || choice < 1 || choice > 6) {
      std::cout << "Invalid input. Please try again." << std::endl;

      util::clear_input_buffer();
    }
    else {
      break;
    }
  } while (true);

  return choice;
}

/*
 * While searching, if the user input word is not in the BST,
 * then this function checks if there are any node that contains
 * the closest word of user input word.
 * It takes a letter and a index variables to keep track of which
 * letter of the input word is currently being checked and in
 * which index is the letter in.
 *
 * - for every letter in the user input word
 *  - pass the root node, letter and the letter index to this function
 *    - if the letter matches the letter in the correct index of a word in a node
 *      - return that node as the current node and repeat
 *        the process for the next letter starting from the current node.
 *    - if the letter doesn't match
 *      - return the last node as closest node
 *
 * Parameters
 * ----------
 * node : bst<word>::Node *
 *  Current node to keep track of which node is the closest.
 *  Starts from the root node.
 * letter : char &
 *  the character in the word to check
 * index : int
 *  the index of the letter in the word
 *
 * Returns
 * -------
 * bst<word>::Node *
 */
ds::bst<word>::Node * find_closest_node(ds::bst<word>::Node * node, const char & letter, const int index) {
  try {
    if (!node) {
      return nullptr;
    }

    std::string current_word = node->data;

    if ( (index + 1) > current_word.size() || letter > current_word[index]) {
      return find_closest_node(node->right.get(), letter, index);
    }
    else if (letter < current_word[index]) {
      return find_closest_node(node->left.get(), letter, index);
    }
    else {
      return node;
    }
  }
  catch (const std::exception & e) {
    std::cerr << "Exception occured : " << e.what() << std::endl;
    return nullptr;
  }
}

void print_suggestions_helper(
  const ds::bst<word>::Node * subtree,
  ds::list<std::string> * suggestion_list,
  std::string matching_substr) {

  try {
    if (!subtree) {
      return;
    }

    print_suggestions_helper(subtree->left.get(), suggestion_list, matching_substr);
    print_suggestions_helper(subtree->right.get(), suggestion_list, matching_substr);

    if(subtree->data.get_term().find(matching_substr) == 0) {
        (*suggestion_list).insert_front(subtree->data.get_term());
    }
  }
  catch (const std::exception & e) {
    std::cerr << "Exception occured : " << e.what() << std::endl;
  }
}

void print_suggestions(word * search) {
  util::clear_screen();

  std::string target_str = *search;
  std::string matching_substr = "";
  ds::bst<word>::Node * current_node = WORD_TREE.get_root();
  ds::bst<word>::Node * temp_node = nullptr;

  std::cout << "                                     Suggestions" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;
  std::cout << "The word '" << *search << "' was not found!" << std::endl << std::endl;

  for(int i = 0; i < target_str.size(); i++) {
    char letter = target_str[i];

    temp_node = find_closest_node(current_node, letter, i);

    if(temp_node == nullptr && i == 0) {
      return;
    }
    else if(temp_node == nullptr) {
      break;
    }
    else {
      current_node = temp_node;
      matching_substr += letter;
    }
  }

  int suggestion_count = 5;
  ds::list<std::string> suggestion_list;

  print_suggestions_helper(current_node, &suggestion_list, matching_substr);

  if(suggestion_list.get_size() < suggestion_count)
    suggestion_count = suggestion_list.get_size();

  if(suggestion_list.get_size() != 0) {
    std::cout << "Did you mean:" << std::endl;

    std::cout << "[ ";
    for(int index = 0; index < suggestion_count; index++) {
      std::cout << suggestion_list[index];

      if(index != suggestion_count - 1)
        std::cout << ", ";
    }
    std::cout << " ]";
  }
}
//------------------------------------------------//
//################################################//


void add_word_helper(std::string * message) {
  std::string terminology = util::input_word(message);

  std::cout << "Enter the definition of the word: " << std::endl;
  std::string definition = util::input_sentence();

  // instantiate a word object named 'new_word'
  word new_word(terminology, definition);

  if(WORD_TREE.search(new_word) == nullptr) {
    if (WORD_TREE.insert(new_word)) {
      std::cout << std::endl;
      std::cout << "Added the word '" << terminology << "' to the Binary Search Tree." << std::endl;

      util::wait_for_input();

      return;
    }
    else {
      std::cout << "Error! Could not add the word '" << terminology << "'." << std::endl;
    }
  }
  else {
    std::cout << "Could not add '" << terminology << "'! It is already in the BST!" << std::endl;

    util::wait_for_input();
  }
}


//################################################//
//------------------------------------------------//
void add_word() {
  util::clear_screen();

  std::cout << "                                        Add Word" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;

  std::string message = "Enter the word you want to add: ";

  add_word_helper(&message);
}

void search_word() {
  util::clear_screen();

  std::string target_str = {};
  ds::bst<word>::Node * root = WORD_TREE.get_root();

  std::cout << "                                          Search" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;

  std::string message = "Enter the word you want to search: ";
  target_str = util::input_word(&message);

  // instantiate a word object named 'target'
  word target(target_str, {});
  ds::bst<word>::Node * found_node = WORD_TREE.search_node(target);

  if(root != nullptr) {
    if(found_node == nullptr) {
      //std::cout << "The word '" << target_str << "' was not found." << std::endl;
      print_suggestions(&target);

      util::wait_for_input();
    }
    else {
      std::cout << std::endl;
      std::cout << "Search result:" << std::endl;

      (*found_node).data.display();

      util::wait_for_input();
    }
  }
}

void delete_word() {
  util::clear_screen();

  std::string terminology = {};

  std::cout << "                                     Delete Word" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;

  std::cout << "Word Tree:" << std::endl;
  WORD_TREE.print();
  std::cout << std::endl;

  std::string message = "Enter the word you want to delete: ";
  terminology = util::input_word(&message);

  word new_word(terminology, {});

  if(!WORD_TREE.search(new_word)) {
    std::cout << "The word '" << terminology << "' was not found!" << std::endl;

    util::wait_for_input();
  }
  else {
    std::string message = "Are you sure you want to delete the word '" + terminology + "' (Yes/No): ";

    if(util::confirmation_check(&message)) {
      if (WORD_TREE.remove(new_word)) {
        std::cout << std::endl;
        std::cout << "The word '" << terminology << "' was deleted successfully!" << std::endl;

        util::wait_for_input();

        return;
      }
      else {
        std::cout << "Error! The word '" << terminology << "' could not be deleted!" << std::endl;

        util::wait_for_input();
      }
    }
  }
}

void update_word() {
  util::clear_screen();

  std::string target_terminology = {};
  std::string terminology = {};

  std::cout << "                                     Update Word" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;

  std::cout << "Word Tree:" << std::endl;
  WORD_TREE.print();
  std::cout << std::endl;


  std::string message = "Enter the word you want to update: ";
  target_terminology = util::input_word(&message);

  word target_word(target_terminology, {});

  if(!WORD_TREE.search(target_word)) {
    std::cout << "Error! The word '" << target_terminology << "' was not found!" << std::endl;

    util::wait_for_input();
  }
  else {
    word word_to_delete = WORD_TREE.search_node(target_word)->data;

    std::cout << std::endl;
    std::cout << "Updating the following word:" << std::endl;

    word_to_delete.display();

    WORD_TREE.remove(word_to_delete);

    std::cout << std::endl;
    message = "Enter the updated word: ";
    add_word_helper(&message);
  }
}

void print_tree() {
  util::clear_screen();

  std::cout << "                                         Display" << std::endl;
  std::cout << "────────────────────────────────────────────────" << std::endl;

  WORD_TREE.print();

  util::wait_for_input();
}
//------------------------------------------------//

//################################################//
