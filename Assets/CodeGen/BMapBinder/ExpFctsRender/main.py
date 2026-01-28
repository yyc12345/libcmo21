import json_loader
import template_render
import utils

def main():
    render = template_render.TemplateRender()
    fcts = json_loader.load_fcts("BMExports.json")

    render.render_cs_expfcts("BMExports.cs", fcts)
    render.render_py_expfcts("BMExports.py", fcts)
    render.render_rs_expfcts("BMExports.rs", fcts)

    print("Done")


if __name__ == "__main__":
    main()
