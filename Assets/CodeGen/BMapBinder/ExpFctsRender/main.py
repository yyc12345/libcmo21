import json_loader
import utils

def main():
    fcts = json_loader.load_fcts("BMExports.json")

    print("Done")


if __name__ == "__main__":
    main()
