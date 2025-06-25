import math
import json
import os

class Node:
    def __init__(self, idx=None, val=None, left=None, right=None, label=None):
        self.idx = idx
        self.val = val
        self.left = left
        self.right = right
        self.label = label

    def is_leaf_node(self):
        return self.label is not None

def _get_majority_class(dataset):
    if not dataset:
        return None 
    categories = [x[-1] for x in dataset]
    counts = {}
    for x in categories:
        counts[x] = counts.get(x, 0) + 1
    return max(counts, key=counts.get)

def get_entropy(dataset):
    if not dataset:
        return 0
        
    categories = [x[-1] for x in dataset]
    counts = dict()

    for x in categories:
        counts[x] = counts.get(x, 0) + 1

    n = len(dataset)
    if n == 0:
        return 0

    entropy = 0.0
    for count in counts.values():
        if count > 0:
            p = count / n
            entropy -= p * math.log2(p)
    return entropy

def get_best_split(dataset):
    best_split_criteria = {'idx': -1, 'val': -1, 'entropy': float('inf'), 'left_ds': None, 'right_ds': None}
    n_features = len(dataset[0]) - 1
    n_total_samples = len(dataset)

    for idx in range(n_features):
        feature_values = sorted(set([row[idx] for row in dataset]))
        
        for val_idx in range(len(feature_values)):
            val = feature_values[val_idx]
            
            left = [x for x in dataset if x[idx] <= val]
            right = [x for x in dataset if x[idx] > val]

            if not left or not right:
                continue

            weight_left = len(left) / n_total_samples
            weight_right = len(right) / n_total_samples
            
            current_entropy = weight_left * get_entropy(left) + weight_right * get_entropy(right)

            if current_entropy < best_split_criteria['entropy']:
                best_split_criteria['idx'] = idx
                best_split_criteria['val'] = val
                best_split_criteria['entropy'] = current_entropy
                best_split_criteria['left_ds'] = left
                best_split_criteria['right_ds'] = right
    
    return best_split_criteria


def build_tree(dataset, current_depth=0, max_depth=None, min_samples_split=2):
    categories = set(x[-1] for x in dataset)
    if len(categories) == 1:
        return Node(label=list(categories)[0])

    if max_depth is not None and current_depth >= max_depth:
        return Node(label=_get_majority_class(dataset))

    if len(dataset) < min_samples_split:
        return Node(label=_get_majority_class(dataset))

    split_criteria = get_best_split(dataset)
    
    if split_criteria['idx'] == -1 or split_criteria['entropy'] == float('inf'):
        return Node(label=_get_majority_class(dataset))

    left_subset = split_criteria['left_ds']
    right_subset = split_criteria['right_ds']

    if not left_subset or not right_subset:
        return Node(label=_get_majority_class(dataset))
        
    left_child = build_tree(left_subset, current_depth + 1, max_depth, min_samples_split)
    right_child = build_tree(right_subset, current_depth + 1, max_depth, min_samples_split)

    return Node(idx=split_criteria['idx'], val=split_criteria['val'], left=left_child, right=right_child)


def print_tree(node, depth=0, feature_names=None):
    if node is None:
        return

    indent = "  " * depth

    if node.is_leaf_node():
        print(f"{indent}Leaf: Predict Class {node.label}")
        return

    feature_display = f"Feature[{node.idx}]"
    if feature_names and node.idx < len(feature_names):
        feature_display = feature_names[node.idx]
    
    val_display = f"{node.val:.2f}" if isinstance(node.val, float) else str(node.val)

    print(f"{indent}{feature_display} <= {val_display}")
    
    print(f"{indent}--> True (Left):")
    print_tree(node.left, depth + 1, feature_names)
    
    print(f"{indent}--> False (Right):")
    print_tree(node.right, depth + 1, feature_names)


def predict_sample(node, sample):
    current_node = node
    while not current_node.is_leaf_node():
        if sample[current_node.idx] <= current_node.val:
            current_node = current_node.left
        else:
            current_node = current_node.right
    return current_node.label


def get_dataset(name):
    filepath = f'datasets/{name}.json'

    with open(filepath, 'r') as f:
        l_dataset = json.load(f)

    l2_dataset = []
    for k, v in l_dataset.items():
        for x in v:
            el = []
            for y in x:
                if isinstance(y, list):
                    for z in y:
                        el.append(z)
                else:
                    el.append(y)
            el.append(k)
            l2_dataset.append(el)
    return l2_dataset





