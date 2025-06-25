import math
import json

class Node:
    def __init__(self, left=None, right=None, idx=None, val=None, label=None):
        self.left = left
        self.right = right
        self.idx = idx 
        self.val = val
        self.label = label


def get_entropy(dataset):
    categories = [x[-1] for x in dataset]
    counts = dict()

    for x in categories:
        counts[x] = counts.get(x, 0) + 1

    n = len(dataset)

    entropy = 0.0
    for count in counts.values():
        p = count / n
        entropy -= p * math.log2(p)

    return entropy



def get_best_split(dataset):
    best_split = [-1, -1, float('inf')]
    n_features = len(dataset[0]) - 1

    for idx in range(n_features):
        values = sorted(set([row[idx] for row in dataset]))
        for val in values:
            left = [x for x in dataset if x[idx] <= val]
            right = [x for x in dataset if x[idx] > val]

            if not left or not right:
                continue

            n = len(dataset)
            weight_left = len(left) / n
            weight_right = len(right) / n
            entropy = weight_left * get_entropy(left) + weight_right * get_entropy(right)

            if entropy < best_split[2]:
                best_split = [idx, val, entropy]

    return best_split
            

def build_tree(dataset):
    categories = set([x[-1] for x in dataset])
    if len(categories) == 1:
        return Node(left=None, right=None, idx=None, val=None), list(categories)[0]

    idx, val, entropy = get_best_split(dataset)

    if idx == -1:
        counts = dict()
        for x in dataset:
            counts[x[-1]] = counts.get(x[-1], 0) + 1
        majority_class = max(counts, key=counts.get)
        return Node(left=None, right=None, idx=None, val=None), majority_class

    left_subset = [x for x in dataset if x[idx] <= val]
    right_subset = [x for x in dataset if x[idx] > val]

    left_node, left_label = build_tree(left_subset)
    right_node, right_label = build_tree(right_subset)

    node = Node(left=left_node, right=right_node, idx=idx, val=val)

    node.left_label = left_label
    node.right_label = right_label

    return node, None

def print_tree(node, depth=0):
    if node is None:
        indent = "  " * depth
        print(f"{indent}None")
        return

    indent = "  " * depth

    if node.label is not None:
        print(f"{indent}Leaf: {node.label}")
        return

    print(f"{indent}Feature[{node.idx}] <= {node.val}")
    
    print(f"{indent}Left:")
    print_tree(node.left, depth + 1)
    
    print(f"{indent}Right:")
    print_tree(node.right, depth + 1)



def get_dataset(name):
    with open(f'datasets/{name}.json', 'r') as f:
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

root, _ = build_tree(get_dataset("stat"))

print_tree(root)
