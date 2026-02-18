use bmap_rs::bmap;

#[test]
fn test_raw() {
    assert!(unsafe { bmap::BMInit() });
    assert!(unsafe { bmap::BMDispose() });
}
