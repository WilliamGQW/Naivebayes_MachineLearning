#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cmath>

using namespace std;

const int row = 28;
const int col = 28;
const int number_of_class = 10;

const double k = 0.1;

struct Image {
    int pixel[row][col];
};

void training(char *training_image_file, char *training_label_file, char *training_model_file);

void classifying(char *training_model_file, char *test_images_file, char *test_label_file, char *test_result_file);

// Auxiary function
// Combine i, j, f, clazz to a string, as a key passed into a map
string generate_str(int i, int j, int f, int clazz);

// Split a string
void split(string &s, string &delim, vector<string> *ret);

// Load file to train
void load_file_data(char *training_image_file, char *training_label_file, vector<int> &ground_truth,
                    vector<Image> &image_vec);

// Save training model to file
void save_model(char *training_model_file, double probability_of_clazz[],
                int number, const map<string, double> &probability_likelihoods_of_clazz_map);

// Load model to classify
void load_model(char *training_model_file, double probability_of_clazz[], int number,
                map<string, double> &probability_likelihoods_of_clazz_map);

// Save classify result to file
void save_result(char *test_result_file, const vector<int> &result_vec);

int main(int argc, char *argv[]) {
    if (argc == 5 && strcmp(argv[1], "-t") == 0) {
        cout << "Training starts..." << endl;
        training(argv[2], argv[3], argv[4]);
        cout << "Training finished..." << endl;
    } else if (argc == 6 && strcmp(argv[1], "-c") == 0) {
        cout << "Classifying starts..." << endl;
        classifying(argv[2], argv[3], argv[4], argv[5]);
        cout << "Classifying finished..." << endl;
    } else {
        cout << "Execute params: \n";
        cout << "Training: /.exe -t data/trainingimages data/traininglabels data/trainingmodels\n";
        cout << "Classifying: /.exe -c data/trainingmodels data/testimages data/testlabels data/testresults\n";
    }

    //  system("pause");
    return 0;
}

string generate_str(int i, int j, int f, int clazz) {
    stringstream ss;
    ss << i << "_";
    ss << j << "_";
    ss << f << "_";
    ss << clazz;

    return ss.str();
}

void save_model(char *training_model_file, double probability_of_clazz[],
                int number, const map<string, double> &probability_likelihoods_of_clazz_map) {
    ofstream ofile(training_model_file, std::ios::out);
    ofile << setprecision(10);
    for (int i = 0; i < number; ++i) {
        ofile << probability_of_clazz[i] << " ";
    }
    ofile << endl;
    ofile << probability_likelihoods_of_clazz_map.size() << endl;

    map<string, double>::const_iterator iter = probability_likelihoods_of_clazz_map.begin();
    while (iter != probability_likelihoods_of_clazz_map.end()) {
        ofile << iter->first << " " << iter->second << endl;
        iter++;
    }

    ofile.flush();
    ofile.close();
    cout << "Model training finished!" << endl;
}

void split(std::string &s, std::string &delim, std::vector<std::string> *ret) {
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != string::npos) {
        ret->push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
        ret->push_back(s.substr(last, index - last));
    }
}

void load_file_data(char *image_file, char *label_file, vector<int> &ground_truth_vec, vector<Image> &image_vec) {
    // Load ground truth
    ifstream fin2(label_file, std::ios::in);
    string s;
    int number_class;
    while (getline(fin2, s)) {
        stringstream word(s);
        word >> number_class;
        ground_truth_vec.push_back(number_class);
    }
    fin2.close();

    int training_size = ground_truth_vec.size();
    ifstream fin1(image_file, std::ios::in);
    // Load image data
    for (int i = 0; i < training_size; ++i) {
        Image img;
        for (int j = 0; j < row; ++j) {
            getline(fin1, s);
            for (int k = 0; k < col; ++k) {
                if (s[k] == ' ') {
                    img.pixel[j][k] = 0;
                } else {
                    img.pixel[j][k] = 1;
                }
            }
        }
        image_vec.push_back(img);
    }
    fin1.close();
}

void training(char *training_image_file, char *training_label_file, char *training_model_file) {
    vector<int> ground_truth_vec;
    vector<Image> training_vec;
    load_file_data(training_image_file, training_label_file, ground_truth_vec, training_vec);
    int training_size = ground_truth_vec.size();

    // Calculate probability of each class
    map<int, int> class_map;
    for (int i = 0; i < training_size; ++i) {
        class_map[ground_truth_vec[i]]++;
    }

    double probability_of_clazz[number_of_class];
    for (int i = 0; i < number_of_class; ++i) {
        probability_of_clazz[i] = class_map[i] / (double) training_size;
    }

    map<string, double> probability_likelihoods_of_clazz_map;
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            for (int f = 0; f <= 1; ++f) {
                for (int clazz = 0; clazz < number_of_class; ++clazz) {
                    int times = 0;
                    int total_number = 0;
                    for (int c = 0; c < training_size; ++c) {
                        if (ground_truth_vec[c] == clazz) {
                            ++total_number;
                            if (training_vec[c].pixel[i][j] == f) {
                                ++times;
                            }
                        }
                    }
                    double result = (double) (k + times) / (2 * k + total_number);
                    string key = generate_str(i, j, f, clazz);
                    probability_likelihoods_of_clazz_map[key] = result;
                }
            }
        }
    }

    save_model(training_model_file, probability_of_clazz, number_of_class, probability_likelihoods_of_clazz_map);
}

void load_model(char *training_model_file, double probability_of_clazz[],
                int number, map<string, double> &probability_likelihoods_of_clazz_map) {
    ifstream fin(training_model_file, std::ios::in);
    string s;
    getline(fin, s);
    vector<string> vec;
    string delim(" ");
    split(s, delim, &vec);
    for (int i = 0; i < number; ++i) {
        stringstream ss;
        ss << vec[i];
        ss >> probability_of_clazz[i];
    }
    getline(fin, s);
    stringstream ss;
    ss << s;
    int row;
    ss >> row;
    for (int i = 0; i < row; ++i) {
        getline(fin, s);
        vector<string> vec;
        split(s, delim, &vec);
        stringstream ss;
        ss << vec[1];
        ss >> probability_likelihoods_of_clazz_map[vec[0]];
    }

    fin.close();
}

void save_result(char *test_result_file, const vector<int> &result_vec) {
    ofstream ofile(test_result_file, std::ios::out);
    for (unsigned int i = 0; i < result_vec.size(); ++i) {
        ofile << result_vec[i] << endl;
    }

    ofile.flush();
    ofile.close();
    cout << "Result is recorded into file" << endl;
}

void classifying(char *training_model_file, char *test_images_file, char *test_label_file, char *test_result_file) {
    double probability_of_clazz[number_of_class];
    map<string, double> probability_likelihoods_of_clazz_map;
    load_model(training_model_file, probability_of_clazz, number_of_class, probability_likelihoods_of_clazz_map);

    vector<int> ground_truth_vec;
    vector<Image> classifying_vec;
    load_file_data(test_images_file, test_label_file, ground_truth_vec, classifying_vec);
    int classifying_size = ground_truth_vec.size();

    vector<int> result_vec;
    vector<double> maximum_probability_vec;
    for (int i = 0; i < classifying_size; ++i) {
        vector<double> posterior_probability_vec;
        for (int clazz = 0; clazz < number_of_class; ++clazz) {
            double result = log(probability_of_clazz[clazz]);
            for (int cRow = 0; cRow < row; ++cRow) {
                for (int cCol = 0; cCol < col; ++cCol) {
                    int f = classifying_vec[i].pixel[cRow][cCol];
                    string key = generate_str(cRow, cCol, f, clazz);
                    result += log(probability_likelihoods_of_clazz_map[key]);
                }
            }
            posterior_probability_vec.push_back(result);
        }
        int maximum_idx = -1;
        double maximum_probability = INT_MIN;
        for (int clazz = 0; clazz < number_of_class; ++clazz) {
            if (posterior_probability_vec[clazz] > maximum_probability) {
                maximum_probability = posterior_probability_vec[clazz];
                maximum_idx = clazz;
            }
        }
        result_vec.push_back(maximum_idx);
        maximum_probability_vec.push_back(maximum_probability);
    }

    // Save result to file
    save_result(test_result_file, result_vec);

    // Calculate correct numbers
    int match_count = 0;
    for (int i = 0; i < classifying_size; ++i) {
        if (result_vec[i] == ground_truth_vec[i]) {
            ++match_count;
        }
    }

    // Calculate confusion matrix
    map<int, int> class_map;
    for (int i = 0; i < classifying_size; ++i) {
        class_map[ground_truth_vec[i]]++;
    }
    double confusion_matrix[number_of_class][number_of_class];
    // Init all elements to 0
    for (int i = 0; i < number_of_class; ++i) {
        for (int j = 0; j < number_of_class; ++j) {
            confusion_matrix[i][j] = 0;
        }
    }
    for (int i = 0; i < classifying_size; ++i) {
        confusion_matrix[ground_truth_vec[i]][result_vec[i]] += 1;
    }
    for (int i = 0; i < number_of_class; ++i) {
        for (int j = 0; j < number_of_class; ++j) {
            confusion_matrix[i][j] /= class_map[i];
            confusion_matrix[i][j] *= 100;
        }
    }

    // Calculate highest and lowest posterior probabilities
    double highest_probabilities[number_of_class];
    double lowest_probabilities[number_of_class];
    for (int i = 0; i < number_of_class; ++i) {
        highest_probabilities[i] = INT_MIN;
        lowest_probabilities[i] = INT_MAX;
    }
    for (int i = 0; i < number_of_class; ++i) {
        for (int j = 0; j < classifying_size; ++j) {
            if (result_vec[j] == i) {
                double value = maximum_probability_vec[j];
                if (highest_probabilities[i] < value) {
                    highest_probabilities[i] = value;
                }
                if (lowest_probabilities[i] > value) {
                    lowest_probabilities[i] = value;
                }
            }
        }
    }

    // Output analysis result
    cout << "Total test count: " << classifying_size << endl;
    cout << "Total matched count: " << match_count << endl;
    cout << "Accuracy: " << (double) match_count * 100 / classifying_size << "%" << endl;
    cout << endl;
    cout << "Confusion matrix: " << endl;

    cout << setprecision(4);
    for (int i = 0; i < number_of_class; ++i) {
        for (int j = 0; j < number_of_class; ++j) {
            cout << confusion_matrix[i][j] << "%\t";
        }
        cout << endl;
    }
    cout << endl;
    cout << "Highest and lowest posterior probabilities for each class:" << endl;
    cout << "Class\tHighest\tLowest" << endl;
    for (int i = 0; i < number_of_class; ++i) {
        cout << i << "\t" << highest_probabilities[i] << "\t" << lowest_probabilities[i] << endl;
    }
}